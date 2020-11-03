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


#include <exception>
#include <iostream>
#include <string>

#include "eaxefx_logger.h"
#include "eaxefx_doom3_fix_lib.h"


auto logger = eaxefx::LoggerUPtr{};

static const auto press_enter_to_exit_message = "Press ENTER to exit.";


std::string get_input_string(
	const char* message = nullptr)
{
	if (message != nullptr)
	{
		logger->info(message);
	}

	auto string = std::string{};
	std::getline(std::cin, string);

	return string;
}


int main(
	int argc,
	char* argv[])
try
{
	static_cast<void>(argc);
	static_cast<void>(argv);

	auto logger_param = eaxefx::LoggerParam{};
	logger_param.skip_message_prefix = true;
	logger_param.has_console_sink = true;
	logger_param.path = "eaxefx_doom3_fix_log.txt";

	logger = eaxefx::make_logger(logger_param);
	logger->set_immediate_mode();

	logger->info("===========================================================================");
	logger->info("Doom 3 EAX fix for visual twitch bug v" EAXEFX_DOOM3_FIX_VERSION);
	logger->info("http://www.pcgamingwiki.com/wiki/Doom_3#Visual_twitch_bug_with_OpenAL.2FEAX");
	logger->info("===========================================================================");
	logger->info("");

	auto doom3_fix = eaxefx::make_doom_3_fix(eaxefx::Doom3FixTarget::file);
	const auto doom3_fix_status = doom3_fix->get_status();

	switch (doom3_fix_status)
	{
		case eaxefx::Doom3FixStatus::patched:
			{
				logger->info("Already patched.");
				logger->info("");

				auto answer = std::string{"\xFF"};

				while (!answer.empty() && answer != "u")
				{
					logger->info("- To unpatch press \"u\" and then \"ENTER\".");
					logger->info("- To skip just press \"ENTER\".");
					answer = get_input_string();
				}

				if (!answer.empty())
				{
					doom3_fix->unpatch();
					logger->info("Succeeded.");
				}
				else
				{
					logger->info("Skipped.");
				}

				get_input_string(press_enter_to_exit_message);
			}
			break;

		case eaxefx::Doom3FixStatus::unpatched:
			{
				auto answer = std::string{"\xFF"};

				logger->info("Not patched.");
				logger->info("");

				while (!(answer.empty() || answer == "p"))
				{
					logger->info("- To patch press \"p\" and then \"ENTER\".");
					logger->info("- To skip just press \"ENTER\".");
					answer = get_input_string();
				}

				if (!answer.empty())
				{
					doom3_fix->patch();
					logger->info("Succeeded.");
				}
				else
				{
					logger->info("Skipped.");
				}

				get_input_string(press_enter_to_exit_message);
			}
			break;

		case eaxefx::Doom3FixStatus::unsupported:
		default:
			logger->info("Not Doom 3 or already applied a different patch.");
			get_input_string(press_enter_to_exit_message);
			break;
	}

	return 0;
}
catch (const std::exception& ex)
{
	if (logger != nullptr)
	{
		logger->error(ex);
	}
	else
	{
		std::cerr << "[ERROR] " << ex.what() << std::endl;
	}

	get_input_string(press_enter_to_exit_message);

	return 1;
}
