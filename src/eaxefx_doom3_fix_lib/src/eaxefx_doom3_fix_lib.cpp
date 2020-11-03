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


#include "eaxefx_doom3_fix_lib.h"

#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
#include <memory>
#include <string_view>
#include <vector>

#include <windows.h>

#include "eaxefx_exception.h"
#include "eaxefx_shared_library.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32Doom3FixException :
	public Exception
{
public:
	explicit Win32Doom3FixException(
		std::string_view message)
		:
		Exception{"DOOM3_FIX_LIB", message}
	{
	}
}; // Win32Doom3FixException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32Doom3Fix :
	public Doom3Fix
{
public:
	Win32Doom3Fix(
		Doom3FixTarget fix_target);


	// ----------------------------------------------------------------------
	// Doom3Fix

	Doom3FixStatus get_status() const noexcept override;

	void patch() override;

	void unpatch() override;

	// Doom3Fix
	// ----------------------------------------------------------------------


private:
	using Data = std::vector<BYTE>;

	struct RefData
	{
		int base_offset{};
		Data data{};
	}; // RefData

	using RefDatas = std::vector<RefData>;


	struct PatchData
	{
		int ref_offset{};
		Data data{};
	}; // PatchData

	using PatchDatas = std::vector<PatchData>;


	bool is_process_{};
	Doom3FixStatus status_{};
	std::filesystem::path path_{};
	RefDatas ref_datas_{};
	PatchDatas patch_datas_{};
	RefDatas patched_ref_datas_{};


	static const char* get_default_file_name() noexcept;


	void make_ref_datas();

	void make_patch_datas();

	void make_patched_ref_data(
		const RefData& ref_data,
		const PatchData& patch_data,
		RefData& patched_ref_data);

	void initialize_datas();


	void get_process_path();

	std::filebuf open_file(
		bool is_writable);

	static bool contains_ref_data(
		std::filebuf& filebuf,
		const RefData& ref_data);

	bool is_unpatched(
		std::filebuf& filebuf);

	bool is_patched(
		std::filebuf& filebuf);

	void get_file_status();

	void initialize();

	void patch_file(
		const RefDatas& ref_datas);

	void patch_process();
}; // Win32Doom3Fix

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Win32Doom3Fix::Win32Doom3Fix(
	Doom3FixTarget fix_target)
{
	switch (fix_target)
	{
		case Doom3FixTarget::file:
			is_process_ = false;
			break;

		case Doom3FixTarget::process:
			is_process_ = true;
			break;

		default:
			throw Win32Doom3FixException{"Unsupported target."};
	}

	initialize();
}

Doom3FixStatus Win32Doom3Fix::get_status() const noexcept
{
	return status_;
}

void Win32Doom3Fix::patch()
{
	switch (status_)
	{
		case Doom3FixStatus::patched:
			throw Win32Doom3FixException{"Already patched."};

		case Doom3FixStatus::unpatched:
			if (is_process_)
			{
				patch_process();
			}
			else
			{
				patch_file(patched_ref_datas_);
			}
			break;

		case Doom3FixStatus::unsupported:
		default:
			throw Win32Doom3FixException{"Unsupported file."};
	}
}

void Win32Doom3Fix::unpatch()
{
	if (is_process_)
	{
		throw Win32Doom3FixException{"Unpatching the process not supported."};
	}

	switch (status_)
	{
		case Doom3FixStatus::patched:
			patch_file(ref_datas_);
			break;

		case Doom3FixStatus::unpatched:
			throw Win32Doom3FixException{"Already unpatched."};

		case Doom3FixStatus::unsupported:
		default:
			throw Win32Doom3FixException{"Unsupported file."};
	}
}

const char* Win32Doom3Fix::get_default_file_name() noexcept
{
	return "DOOM3.exe";
}

void Win32Doom3Fix::make_ref_datas()
{
	ref_datas_ =
	{
		{
			0xEAF3F,

			{
				0x74, 0x18,
				0xA1, 0xA4, 0xFF, 0x7C, 0x00,
				0x8B, 0x08,
				0x68, 0xE4, 0x34, 0x77, 0x00,
				0x50,
				0xFF, 0x51, 0x5C,
				0x83, 0xC4, 0x08,
				0xE9, 0xC5, 0x00, 0x00, 0x00,
			},
		},

		{
			0xEB270,

			{
				0x74, 0x18,
				0xA1, 0xA4, 0xFF, 0x7C, 0x00,
				0x8B, 0x08,
				0x68, 0xE4, 0x34, 0x77, 0x00,
				0x50,
				0xFF, 0x51, 0x5C,
				0x83, 0xC4, 0x08,
				0xE9, 0xEE, 0x00, 0x00, 0x00,
			},
		},

		{
			0xEB38F,

			{
				0x8B, 0x44, 0x24, 0x10,
				0x8A, 0x48, 0x4C,
				0x84, 0xC9,
				0x5D,
				0x5B,
				0x74, 0x19,
			},
		},
	};
}

void Win32Doom3Fix::make_patch_datas()
{
	patch_datas_ =
	{
		{
			0,

			{
				0x0F, 0x84, 0xCE, 0x00, 0x00, 0x00,
			},
		},


		{
			0,

			{
				0x0F, 0x84, 0xF6, 0x00, 0x00, 0x00,
			},
		},

		{
			0x0B,

			{
				0xEB,
			},
		},
	};
}

void Win32Doom3Fix::initialize_datas()
{
	make_ref_datas();
	make_patch_datas();

	const auto count = ref_datas_.size();
	patched_ref_datas_.resize(count);

	for (auto i = std::size_t{}; i < count; ++i)
	{
		make_patched_ref_data(ref_datas_[i], patch_datas_[i], patched_ref_datas_[i]);
	}
}

void Win32Doom3Fix::get_process_path()
{
	constexpr auto max_string_length = 4'095;
	constexpr auto max_string_size = max_string_length + 1;
	using Buffer = std::array<WCHAR, max_string_size>;

	auto buffer = Buffer{};

	static_cast<void>(GetModuleFileNameW(
		nullptr,
		buffer.data(),
		max_string_length));

	path_ = std::filesystem::path{buffer.data()};

	if (path_.empty())
	{
		throw Win32Doom3FixException{"Failed to get a path of the current process."};
	}
}

void Win32Doom3Fix::make_patched_ref_data(
	const RefData& ref_data,
	const PatchData& patch_data,
	RefData& patched_ref_data)
{
	patched_ref_data = ref_data;

	std::uninitialized_copy_n(
		patch_data.data.cbegin(),
		patch_data.data.size(),
		patched_ref_data.data.begin() + patch_data.ref_offset
	);

}

std::filebuf Win32Doom3Fix::open_file(
	bool is_writable)
{
	const auto mode = static_cast<std::ios_base::openmode>(
		std::ios_base::binary |
		std::ios_base::in |
		(is_writable ? std::ios_base::out : 0)
	);

	auto filebuf = std::filebuf{};
	filebuf.open(path_, mode);

	if (!filebuf.is_open())
	{
		const auto& utf8_path = path_.u8string();
		throw Win32Doom3FixException{"Failed to open file \"" + utf8_path + "\"."};
	}

	return filebuf;
}

bool Win32Doom3Fix::contains_ref_data(
	std::filebuf& filebuf,
	const RefData& ref_data)
{
	auto file_data = ref_data.data;
	std::fill(file_data.begin(), file_data.end(), Data::value_type{});

	filebuf.pubseekpos(ref_data.base_offset);
	filebuf.sgetn(reinterpret_cast<char*>(file_data.data()), file_data.size());

	return file_data == ref_data.data;
}

bool Win32Doom3Fix::is_unpatched(
	std::filebuf& filebuf)
{
	for (const auto& ref_data : ref_datas_)
	{
		if (!contains_ref_data(filebuf, ref_data))
		{
			return false;
		}
	}

	return true;
}

bool Win32Doom3Fix::is_patched(
	std::filebuf& filebuf)
{
	for (const auto& patched_ref_data : patched_ref_datas_)
	{
		if (!contains_ref_data(filebuf, patched_ref_data))
		{
			return false;
		}
	}

	return true;
}

void Win32Doom3Fix::get_file_status()
{
	auto filebuf = open_file(false);

	if (is_patched(filebuf))
	{
		status_ = Doom3FixStatus::patched;
	}
	else if (is_unpatched(filebuf))
	{
		status_ = Doom3FixStatus::unpatched;
	}
	else
	{
		status_ = Doom3FixStatus::unsupported;
	}
}

void Win32Doom3Fix::initialize()
{
	if (is_process_)
	{
		get_process_path();
	}
	else
	{
		path_ = get_default_file_name();
	}

	initialize_datas();
	get_file_status();
}

void Win32Doom3Fix::patch_file(
	const RefDatas& ref_datas)
{
	auto filebuf = open_file(true);

	for (const auto& ref_data : ref_datas)
	{
		const auto new_position = filebuf.pubseekpos(ref_data.base_offset);

		if (static_cast<std::streamoff>(new_position) < 0)
		{
			throw Win32Doom3FixException{"Failed to set new file position."};
		}

		const auto data_size = ref_data.data.size();

		const auto written_size = filebuf.sputn(
			reinterpret_cast<const char*>(ref_data.data.data()),
			data_size
		);

		if (written_size != static_cast<std::streamoff>(data_size))
		{
			throw Win32Doom3FixException{"I/O write error."};
		}
	}
}

void Win32Doom3Fix::patch_process()
{
	const auto process_handle = GetCurrentProcess();
	const auto base_address = reinterpret_cast<BYTE*>(GetModuleHandleW(nullptr));

	for (const auto& patched_ref_data : patched_ref_datas_)
	{
		auto old_protect = DWORD{PAGE_EXECUTE_READ};

		const auto patch_address = base_address + patched_ref_data.base_offset;
		const auto patch_size = patched_ref_data.data.size();

		VirtualProtect(
			patch_address,
			patch_size,
			PAGE_READWRITE,
			&old_protect);

		std::uninitialized_copy_n(
			patched_ref_data.data.cbegin(),
			patch_size,
			patch_address
		);

		auto new_old_protect = DWORD{};

		VirtualProtect(
			patch_address,
			patch_size,
			old_protect,
			&new_old_protect);

		FlushInstructionCache(
			process_handle,
			patch_address,
			patch_size);
	}
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using Doom3FixUPtr = std::unique_ptr<Doom3Fix>;


Doom3FixUPtr make_doom_3_fix(
	Doom3FixTarget fix_target)
{
	return std::make_unique<Win32Doom3Fix>(fix_target);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
