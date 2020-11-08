/*

EAX OpenAL Extension

Copyright (c) 2020 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.

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


#include "eaxefx_shared_library.h"

#include <windows.h>

#include <memory>
#include <string_view>

#include "eaxefx_encoding.h"
#include "eaxefx_exception.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32SharedLibraryException :
	public Exception
{
public:
	explicit Win32SharedLibraryException(
		std::string_view message)
		:
		Exception{"WIN32_SHARED_LIBRARY", message}
	{
	}
}; // Win32SharedLibraryException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class SharedLibraryImpl :
	public SharedLibrary
{
public:
	SharedLibraryImpl(
		const String& path);

	~SharedLibraryImpl() override;


	void* resolve(
		const char* symbol_name) noexcept override;


private:
	HMODULE win32_module_;
}; // SharedLibraryImpl

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

SharedLibraryImpl::SharedLibraryImpl(
	const String& path)
	:
	win32_module_{}
{
	const auto u16_path = encoding::to_utf16(path);
	win32_module_ = LoadLibraryW(reinterpret_cast<LPCWSTR>(u16_path.c_str()));

	if (win32_module_ == nullptr)
	{
		throw Win32SharedLibraryException{"LoadLibraryW failed."};
	}
}

SharedLibraryImpl::~SharedLibraryImpl()
{
	if (win32_module_ != nullptr)
	{
		static_cast<void>(FreeLibrary(win32_module_));
	}
}

void* SharedLibraryImpl::resolve(
	const char* symbol_name) noexcept
{
	return reinterpret_cast<void*>(GetProcAddress(win32_module_, symbol_name));
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

SharedLibraryUPtr make_shared_library(
	const String& path)
{
	return std::make_unique<SharedLibraryImpl>(path);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



} // eaxefx
