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


#ifndef EAXEFX_CONDITION_VARIABLE_INCLUDED
#define EAXEFX_CONDITION_VARIABLE_INCLUDED


#include <chrono>
#include <memory>
#include <mutex>

#include "eaxefx_mutex.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class ConditionVariable
{
public:
	using UniqueLock = std::unique_lock<Mutex>;


	ConditionVariable();

	~ConditionVariable();


	void wait(
		UniqueLock& unique_lock);

	bool wait_for(
		UniqueLock& unique_lock,
		std::chrono::milliseconds timeout_ms);

	void notify_one();


private:
	class Impl;
	class ImplCs;
	class ImplSrw;

	using ImplUPtr = std::unique_ptr<Impl>;
	ImplUPtr impl_;
}; // ConditionVariable

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_CONDITION_VARIABLE_INCLUDED
