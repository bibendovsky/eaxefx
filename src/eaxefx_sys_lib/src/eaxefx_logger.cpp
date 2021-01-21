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


#include "eaxefx_logger.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <deque>
#include <exception>
#include <iterator>
#include <memory>
#include <mutex>
#include <string_view>

#include "eaxefx_condition_variable.h"
#include "eaxefx_console.h"
#include "eaxefx_file.h"
#include "eaxefx_mutex.h"
#include "eaxefx_process.h"
#include "eaxefx_system_time.h"
#include "eaxefx_thread.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void Logger::info(
	const String& message) noexcept
{
	write(LoggerMessageType::info, message);
}

void Logger::warning(
	const String& message) noexcept
{
	write(LoggerMessageType::warning, message);
}

void Logger::error(
	const String& message) noexcept
{
	write(LoggerMessageType::error, message);
}

void Logger::error(
	const std::exception& exception) noexcept
{
	write(exception);
}

void Logger::error(
	const std::exception& exception,
	const String& message) noexcept
{
	error(message);
	write(exception);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct LoggerMessage
{
	LoggerMessageType type{};
	String message{};


	LoggerMessage(
		LoggerMessageType type,
		const String& message)
		:
		type{type},
		message{message}
	{
	}
}; // LoggerMessage

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class LoggerImpl final :
	public Logger
{
public:
	LoggerImpl(
		const LoggerParam& param);

	~LoggerImpl() override;


	void flush() noexcept override;

	void set_immediate_mode() noexcept override;


	void write(
		LoggerMessageType message_type,
		const String& message) noexcept override;

	void write(
		const std::exception& ex) noexcept override;


private:
	static constexpr auto min_level = 0;
	static constexpr auto max_level = 32;

	using Messages = std::deque<LoggerMessage>;


	bool skip_message_prefix_{};
	bool is_shared_library_{};
	Console* console_{};
	String path_{};
	String timestamp_buffer_{};
	String message_buffer_{};
	bool has_messages_{};
	bool is_flushing_{};
	bool is_flushing_ack_{};
	bool is_quit_thread_{};
	bool is_quit_thread_ack_{};
	Messages messages_{};
	Messages mt_messages_{};
	Mutex mutex_{};
	ConditionVariable cv_{};
	ConditionVariable cv_ack_{};
	ThreadUPtr thread_{};


	void clear_log_file();

	static String make_timestamp_buffer();

	static String make_message_buffer();

	void write_messages(
		Messages& messages) noexcept;

	void write_messages() noexcept;

	static void thread_func_proxy(
		void* arg);

	void thread_func();

	void write_internal(
		const std::exception& ex);

	void set_immediate_mode_internal() noexcept;
}; // LoggerImpl

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

LoggerImpl::LoggerImpl(
	const LoggerParam& param)
	:
	skip_message_prefix_{param.skip_message_prefix},
	is_shared_library_{process::is_shared_library()},
	console_{param.console},
	path_{param.path},
	timestamp_buffer_{make_timestamp_buffer()},
	message_buffer_{make_message_buffer()},
	mutex_{},
	cv_{},
	cv_ack_{}
{
	clear_log_file();

	thread_ = make_thread(thread_func_proxy, this);
}

LoggerImpl::~LoggerImpl()
{
	set_immediate_mode_internal();
}

void LoggerImpl::flush() noexcept
{
	auto is_notify = false;

	{
		const auto lock = std::scoped_lock{mutex_};

		if (is_quit_thread_)
		{
			write_messages();
		}
		else
		{
			is_notify = true;
			is_flushing_ = true;
			is_flushing_ack_ = false;
		}
	}

	if (is_notify)
	{
		cv_.notify_one();

		auto lock = std::unique_lock{mutex_};

		while (!is_flushing_ack_)
		{
			cv_ack_.wait(lock);
		}
	}
}

void LoggerImpl::set_immediate_mode() noexcept
{
	set_immediate_mode_internal();
}

void LoggerImpl::write(
	LoggerMessageType message_type,
	const String& message) noexcept
try
{
	{
		const auto lock = std::scoped_lock{mutex_};
		messages_.emplace_back(LoggerMessage{message_type, message});

		if (is_quit_thread_)
		{
			write_messages();
		}
		else
		{
			has_messages_ = true;
			cv_.notify_one();
		}
	}
}
catch (...)
{
}

void LoggerImpl::write(
	const std::exception& ex) noexcept
try
{
	write_internal(ex);
}
catch (...)
{
}

void LoggerImpl::clear_log_file()
try
{
	make_file(path_, FileOpenMode{file_open_mode_write | file_open_mode_truncate});
}
catch (const std::exception&)
{
}

String LoggerImpl::make_timestamp_buffer()
{
	auto message_buffer = String{};
	message_buffer.resize(23);
	return message_buffer;
}

String LoggerImpl::make_message_buffer()
{
	auto message_buffer = String{};
	message_buffer.reserve(2048);
	return message_buffer;
}

void LoggerImpl::write_messages(
	Messages& messages) noexcept
try
{
	if (messages.empty())
	{
		return;
	}

	auto file = make_file(path_, FileOpenMode{file_open_mode_write});
	file->move_to_the_end();

	for (const auto& message : messages)
	{
		auto is_error_for_console = false;

		const auto& system_time = make_system_time();
		make_system_time_string(system_time, timestamp_buffer_);

		message_buffer_.clear();

		if (!skip_message_prefix_)
		{
			message_buffer_ += '[';
			message_buffer_ += timestamp_buffer_;
			message_buffer_ += "] ";
			message_buffer_ += "[EAXEFX] ";
			message_buffer_ += '[';

			switch (message.type)
			{
				case LoggerMessageType::info:
					message_buffer_ += 'I';
					break;

				case LoggerMessageType::warning:
					is_error_for_console = true;
					message_buffer_ += 'W';
					break;

				case LoggerMessageType::error:
					is_error_for_console = true;
					message_buffer_ += 'E';
					break;

				default:
					is_error_for_console = true;
					message_buffer_ += '?';
					break;
			}

			message_buffer_ += ']';
		}

		if (!message.message.empty())
		{
			if (!message_buffer_.empty())
			{
				message_buffer_ += ' ';
			}

			message_buffer_ += message.message;
		}

		message_buffer_ += '\n';

		file->write(message_buffer_.c_str(), static_cast<int>(message_buffer_.size()));

		if (console_ != nullptr)
		{
			if (is_error_for_console)
			{
				console_->write_error(message_buffer_);
			}
			else
			{
				console_->write(message_buffer_);
			}
		}
	}

	if (console_ != nullptr && !messages.empty())
	{
		console_->flush();
	}

	messages.clear();
}
catch (...)
{
}

void LoggerImpl::write_messages() noexcept
{
	write_messages(mt_messages_);
	write_messages(messages_);
}

void LoggerImpl::thread_func_proxy(
	void* arg)
{
	static_cast<LoggerImpl*>(arg)->thread_func();
}

void LoggerImpl::thread_func()
try
{
	thread::set_name("eaxefx_logger");

	while (true)
	{
		auto is_flushing = false;

		{
			auto lock = std::unique_lock{mutex_};

			while (!is_quit_thread_ && !is_flushing_ && !has_messages_)
			{
				cv_.wait(lock);
			}

			has_messages_ = false;

			if (is_quit_thread_)
			{
				break;
			}

			is_flushing = is_flushing_;

			mt_messages_.insert(
				mt_messages_.end(),
				std::make_move_iterator(messages_.begin()),
				std::make_move_iterator(messages_.end())
			);

			messages_.clear();
		}

		write_messages(mt_messages_);

		if (is_flushing)
		{
			const auto lock = std::scoped_lock{mutex_};
			is_flushing_ = false;
			is_flushing_ack_ = true;
			cv_ack_.notify_one();
		}
	}

	{
		auto lock = std::scoped_lock{mutex_};
		is_quit_thread_ack_ = true;
		cv_ack_.notify_one();
	}
}
catch (const std::exception&)
{
}

void LoggerImpl::write_internal(
	const std::exception& ex)
{
	error(ex.what());
}

void LoggerImpl::set_immediate_mode_internal() noexcept
try
{
	auto is_notify = false;

	{
		const auto lock = std::scoped_lock{mutex_};

		if (!is_quit_thread_)
		{
			is_notify = true;
			is_quit_thread_ = true;
			cv_.notify_one();
		}
	}

	if (is_notify)
	{
		auto lock = std::unique_lock{mutex_};

		if (is_shared_library_)
		{
			// If logger instance was created inside DLL we can't wait indefinitely.
			// Threads could be already terminated.

			constexpr auto wait_duration_ms = std::chrono::milliseconds{10};

			cv_ack_.wait_for(lock, wait_duration_ms);
		}
		else
		{
			while (!is_quit_thread_ack_)
			{
				cv_ack_.wait(lock);
			}
		}
	}

	write_messages();
}
catch (...)
{
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

LoggerUPtr make_logger(
	const LoggerParam& param)
{
	return std::make_unique<LoggerImpl>(param);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
