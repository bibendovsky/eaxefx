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


#include "eaxefx_thread.h"

#include <windows.h>
#include <process.h>

#include <cassert>

#include "eaxefx_encoding.h"
#include "eaxefx_exception.h"
#include "eaxefx_shared_library.h"
#include "eaxefx_string.h"


namespace eaxefx::thread
{


void set_name(
	const char* utf8_name)
{
	assert(utf8_name);

	auto shared_library = make_shared_library("kernel32.dll");

	using SetThreadDescriptionFunc = HRESULT (WINAPI*)(
		HANDLE hThread,
		PCWSTR lpThreadDescription
	);

	const auto set_thread_description_func =
		reinterpret_cast<SetThreadDescriptionFunc>(shared_library->resolve("SetThreadDescription"));

	if (!set_thread_description_func)
	{
		return;
	}

	const auto thread_handle = GetCurrentThread();

	const auto& utf16_name = encoding::to_utf16(utf8_name);

	set_thread_description_func(
		thread_handle,
		reinterpret_cast<LPCWSTR>(utf16_name.c_str())
	);
}


} // eaxefx::thread


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class ThreadException :
	public Exception
{
public:
	explicit ThreadException(
		const char* message)
		:
		Exception{"WIN32_THREAD", message}
	{
	}
}; // ThreadException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class ThreadImpl final :
	public Thread
{
public:
	ThreadImpl(
		ThreadFunction thread_function,
		void* thread_argument);

	~ThreadImpl() override;


	// ----------------------------------------------------------------------
	// Thread

	// Thread
	// ----------------------------------------------------------------------


private:
	HANDLE handle_;
	ThreadFunction thread_function_{};
	void* thread_argument_{};


	static unsigned int __stdcall proxy_thread_function(
		void* user_data);

	void thread_function();
}; // ThreadImpl

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

ThreadImpl::ThreadImpl(
	ThreadFunction thread_function,
	void* thread_argument)
	:
	thread_function_{thread_function},
	thread_argument_{thread_argument}
{
	if (!thread_function)
	{
		throw ThreadException{"Null thread function."};
	}

	constexpr auto stack_size = 1U << 20;

	handle_ = reinterpret_cast<HANDLE>(::_beginthreadex(
		nullptr,
		stack_size,
		proxy_thread_function,
		this,
		CREATE_SUSPENDED,
		nullptr
	));

	if (!handle_)
	{
		throw ThreadException{"Failed to create a thread."};
	}

	const auto resume_result = ResumeThread(handle_);
	constexpr auto invalid_resume_result = DWORD{} - DWORD{1};

	if (resume_result == invalid_resume_result)
	{
		throw ThreadException{"Failed to resume a thread."};
	}
}

ThreadImpl::~ThreadImpl()
{
	if (handle_)
	{
		constexpr auto timeout_ms = 10;

		WaitForSingleObject(handle_, timeout_ms);
		CloseHandle(handle_);
	}
}

unsigned int __stdcall ThreadImpl::proxy_thread_function(
	void* user_data)
{
	auto thread = static_cast<ThreadImpl*>(user_data);

	try
	{
		thread->thread_function();
	}
	catch (...)
	{
	}

	_endthreadex(0);

	return 0;
}

void ThreadImpl::thread_function()
{
	thread_function_(thread_argument_);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

ThreadUPtr make_thread(
	ThreadFunction thread_function,
	void* thread_argument)
{
	return std::make_unique<ThreadImpl>(thread_function, thread_argument);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
