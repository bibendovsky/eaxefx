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


#ifndef EAXEFX_FS_PATH_INCLUDED
#define EAXEFX_FS_PATH_INCLUDED


#include <string>

#include "eaxefx_platform.h"
#include "eaxefx_core_types.h"


namespace eaxefx::fs
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

constexpr auto native_separator =
#if EAXEFX_WIN32
		'\\'
#endif // EAXEFX_WIN32
;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Path
{
public:
	Path();

	Path(
		const char* rhs_path,
		Int rhs_size);

	explicit Path(
		const char* rhs_path);

	Path(
		const Path& rhs_path);

	Path(
		Path&& rhs_path) noexcept;


	Path& operator=(
		const char* rhs_path);

	Path& operator=(
		const Path& rhs_path);

	Path& operator=(
		Path&& rhs_path) noexcept;


	bool is_empty() const noexcept;

	const char* get_data() const noexcept;

	Int get_size() const noexcept;


	Path& assign(
		const char* rhs_path,
		Int rhs_size);

	Path& assign(
		const char* rhs_path);

	Path& assign(
		const Path& rhs_path);


	Path& append(
		const char* rhs_path,
		Int rhs_size);

	Path& append(
		const char* rhs_path);

	Path& append(
		const Path& rhs_path);


	void clear();


private:
	struct CapacityTag{};
	struct SizeTag{};


	using Data = std::string;


	Data data_{};


	friend Path operator/(
		const char* lhs_path,
		const Path& rhs_path);

	friend Path operator/(
		const Path& lhs_path,
		const char* rhs_path);

	friend Path operator/(
		const Path& lhs_path,
		const Path& rhs_path);


	Path(
		CapacityTag,
		Int capacity);

	Path(
		SizeTag,
		Int size);

	void normalize_separators(
		Int offset);

	void set_capacity(
		Int size);

	void add_capacity(
		Int size);

	void resize(
		Int size);
}; // Path

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Path& operator/=(
	Path& lhs,
	const char* rhs_path);

Path& operator/=(
	Path& lhs,
	const Path& rhs_path);


Path operator/(
	const char* lhs_path,
	const Path& rhs_path);

Path operator/(
	const Path& lhs_path,
	const char* rhs_path);

Path operator/(
	const Path& lhs_path,
	const Path& rhs_path);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::fs


#endif // !EAXEFX_FS_PATH_INCLUDED
