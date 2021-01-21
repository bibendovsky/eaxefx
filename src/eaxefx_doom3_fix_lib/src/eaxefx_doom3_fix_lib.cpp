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


#include "eaxefx_doom3_fix_lib.h"

#include <algorithm>
#include <array>
#include <memory>
#include <vector>

#include <windows.h>

#include "eaxefx_encoding.h"
#include "eaxefx_exception.h"
#include "eaxefx_file.h"
#include "eaxefx_shared_library.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Doom3FixLibException :
	public Exception
{
public:
	explicit Doom3FixLibException(
		const char* message)
		:
		Exception{"DOOM3_FIX_LIB", message}
	{
	}
}; // Doom3FixLibException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Doom3FixImpl :
	public Doom3Fix
{
public:
	Doom3FixImpl(
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
	String path_{};
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

	FileUPtr open_file(
		bool is_writable);

	static bool contains_ref_data(
		File* file,
		const RefData& ref_data);

	bool is_unpatched(
		File* file);

	bool is_patched(
		File* file);

	void get_file_status();

	void initialize();

	void patch_file(
		const RefDatas& ref_datas);

	void patch_process();
}; // Doom3FixImpl

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Doom3FixImpl::Doom3FixImpl(
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
			throw Doom3FixLibException{"Unsupported target."};
	}

	initialize();
}

Doom3FixStatus Doom3FixImpl::get_status() const noexcept
{
	return status_;
}

void Doom3FixImpl::patch()
{
	switch (status_)
	{
		case Doom3FixStatus::patched:
			throw Doom3FixLibException{"Already patched."};

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
			throw Doom3FixLibException{"Unsupported file."};
	}
}

void Doom3FixImpl::unpatch()
{
	if (is_process_)
	{
		throw Doom3FixLibException{"Unpatching the process not supported."};
	}

	switch (status_)
	{
		case Doom3FixStatus::patched:
			patch_file(ref_datas_);
			break;

		case Doom3FixStatus::unpatched:
			throw Doom3FixLibException{"Already unpatched."};

		case Doom3FixStatus::unsupported:
		default:
			throw Doom3FixLibException{"Unsupported file."};
	}
}

const char* Doom3FixImpl::get_default_file_name() noexcept
{
	return "DOOM3.exe";
}

void Doom3FixImpl::make_ref_datas()
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

void Doom3FixImpl::make_patch_datas()
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

void Doom3FixImpl::initialize_datas()
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

void Doom3FixImpl::get_process_path()
{
	constexpr auto max_string_length = 4'095;
	constexpr auto max_string_size = max_string_length + 1;
	using Buffer = std::array<char16_t, max_string_size>;

	auto buffer = Buffer{};

	static_cast<void>(GetModuleFileNameW(
		nullptr,
		reinterpret_cast<WCHAR*>(buffer.data()),
		max_string_length)
	);

	path_ = encoding::to_utf8(buffer.data());

	if (path_.empty())
	{
		throw Doom3FixLibException{"Failed to get a path of the current process."};
	}
}

void Doom3FixImpl::make_patched_ref_data(
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

FileUPtr Doom3FixImpl::open_file(
	bool is_writable)
{
	const auto open_mode = static_cast<FileOpenMode>(
		file_open_mode_read |
		(is_writable ? file_open_mode_write : file_open_mode_none)
	);

	return make_file(path_.c_str(), open_mode);
}

bool Doom3FixImpl::contains_ref_data(
	File* file,
	const RefData& ref_data)
{
	auto file_data = ref_data.data;
	std::fill(file_data.begin(), file_data.end(), Data::value_type{});

	file->set_position(ref_data.base_offset);
	file->read(file_data.data(), file_data.size());

	return file_data == ref_data.data;
}

bool Doom3FixImpl::is_unpatched(
	File* file)
{
	for (const auto& ref_data : ref_datas_)
	{
		if (!contains_ref_data(file, ref_data))
		{
			return false;
		}
	}

	return true;
}

bool Doom3FixImpl::is_patched(
	File* file)
{
	for (const auto& patched_ref_data : patched_ref_datas_)
	{
		if (!contains_ref_data(file, patched_ref_data))
		{
			return false;
		}
	}

	return true;
}

void Doom3FixImpl::get_file_status()
{
	auto file = open_file(false);

	if (is_patched(file.get()))
	{
		status_ = Doom3FixStatus::patched;
	}
	else if (is_unpatched(file.get()))
	{
		status_ = Doom3FixStatus::unpatched;
	}
	else
	{
		status_ = Doom3FixStatus::unsupported;
	}
}

void Doom3FixImpl::initialize()
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

void Doom3FixImpl::patch_file(
	const RefDatas& ref_datas)
{
	auto file = open_file(true);

	for (const auto& ref_data : ref_datas)
	{
		file->set_position(ref_data.base_offset);

		const auto data_size = static_cast<int>(ref_data.data.size());

		const auto written_size = file->write(ref_data.data.data(), data_size);

		if (written_size != data_size)
		{
			throw Doom3FixLibException{"I/O write error."};
		}
	}
}

void Doom3FixImpl::patch_process()
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
	return std::make_unique<Doom3FixImpl>(fix_target);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
