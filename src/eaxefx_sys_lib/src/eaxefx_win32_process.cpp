/*

EAX OpenAL Extension

Copyright (c) 2020-2021 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

*/


#include "eaxefx_process.h"

#include <windows.h>

#include <algorithm>
#include <utility>
#include <vector>

#include "eaxefx_shared_library.h"


namespace eaxefx::process
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

bool is_shared_library() noexcept
{
	const auto root_module = ::GetModuleHandleW(nullptr);

	auto current_module = ::HMODULE{};

	const auto winapi_result = ::GetModuleHandleExW(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<::LPCWSTR>(is_shared_library),
		&current_module
	);

	if (!winapi_result)
	{
		return false;
	}

	return root_module != current_module;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void* get_module_address(
	std::string_view module_name) noexcept
try
{
	using EnumProcessModulesFunc = ::BOOL (WINAPI *)(
		::HANDLE hProcess,
		::HMODULE* lphModule,
		::DWORD cb,
		::LPDWORD lpcbNeeded
	);

	using GetModuleBaseNameFunc = ::DWORD (WINAPI *)(
		::HANDLE hProcess,
		::HMODULE hModule,
		::LPSTR lpFilename,
		::DWORD nSize
	);

	auto enum_process_modules_func = EnumProcessModulesFunc{};
	auto get_module_base_name_func = GetModuleBaseNameFunc{};

	auto kernel32_library = SharedLibraryUPtr{};
	auto psapi_library = SharedLibraryUPtr{};

	if (!enum_process_modules_func || !get_module_base_name_func)
	{
		kernel32_library = make_shared_library("kernel32.dll");

		enum_process_modules_func = reinterpret_cast<EnumProcessModulesFunc>(
			kernel32_library->resolve("K32EnumProcessModules")
		);

		get_module_base_name_func = reinterpret_cast<GetModuleBaseNameFunc>(
			kernel32_library->resolve("K32GetModuleBaseNameA")
		);
	}

	if (!enum_process_modules_func || !get_module_base_name_func)
	{
		psapi_library = make_shared_library("psapi.dll");

		enum_process_modules_func = reinterpret_cast<EnumProcessModulesFunc>(
			psapi_library->resolve("EnumProcessModules")
		);

		get_module_base_name_func = reinterpret_cast<GetModuleBaseNameFunc>(
			psapi_library->resolve("GetModuleBaseNameA")
		);
	}

	if (!enum_process_modules_func || !get_module_base_name_func)
	{
		return nullptr;
	}

	const auto process_handle = ::GetCurrentProcess();

	auto max_module_count = ::DWORD{};

	const auto max_count_result = enum_process_modules_func(
		process_handle,
		nullptr,
		0,
		&max_module_count
	);

	if (max_count_result == FALSE)
	{
		return nullptr;
	}

	using Modules = std::vector<::HMODULE>;
	auto modules = Modules{};
	modules.resize(max_module_count);

	auto module_count = ::DWORD{};

	const auto module_count_result = enum_process_modules_func(
		process_handle,
		modules.data(),
		max_module_count,
		&module_count
	);

	if (module_count_result == FALSE)
	{
		return nullptr;
	}

	modules.resize(module_count);

	const auto module_name_size = static_cast<::DWORD>(module_name.size());

	const auto to_lower = [](
		char ch)
	{
		if (ch >= 'A' && ch <= 'Z')
		{
			return static_cast<char>('a' + ch - 'A');
		}
		else
		{
			return ch;
		}
	};

	const auto compare_char = [to_lower](
		char lhs_ch,
		char rhs_ch)
	{
		return to_lower(lhs_ch) == to_lower(rhs_ch);
	};

	char found_module_name[MAX_PATH];

	for (const auto module : modules)
	{
		const auto found_module_name_size = get_module_base_name_func(
			process_handle,
			module,
			found_module_name,
			MAX_PATH
		);

		if (found_module_name_size != module_name_size)
		{
			continue;
		}

		const auto are_names_equal = std::equal(
			module_name.cbegin(),
			module_name.cend(),
			found_module_name,
			compare_char
		);

		if (are_names_equal)
		{
			return module;
		}
	}

	return nullptr;
}
catch (...)
{
	return nullptr;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::process
