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


#include <exception>

#include "eaxefx_console.h"
#include "eaxefx_string.h"

#include "eaxefx_doom3_fix_lib.h"


auto console = eaxefx::ConsoleUPtr{};

static const auto press_enter_to_exit_message = "Press ENTER to exit.\n";


eaxefx::String get_input_string(
	const char* message = nullptr)
{
	if (console == nullptr)
	{
		return eaxefx::String{};
	}

	if (message != nullptr)
	{
		console->write(message);
	}

	return console->read_line();
}


int main(
	int argc,
	char* argv[])
try
{
	static_cast<void>(argc);
	static_cast<void>(argv);

	console = eaxefx::make_console();

	console->write("===========================================================================\n");
	console->write("Doom 3 EAX fix for visual twitch bug v" EAXEFX_DOOM3_FIX_VERSION "\n");
	console->write("http://www.pcgamingwiki.com/wiki/Doom_3#Visual_twitch_bug_with_OpenAL.2FEAX\n");
	console->write("===========================================================================\n");
	console->write("\n");

	auto doom3_fix = eaxefx::make_doom_3_fix(eaxefx::Doom3FixTarget::file);
	const auto doom3_fix_status = doom3_fix->get_status();

	switch (doom3_fix_status)
	{
		case eaxefx::Doom3FixStatus::patched:
			{
				console->write("Already patched.\n");
				console->write("\n");

				auto answer = eaxefx::String{"\xFF"};

				while (!answer.empty() && answer != "u")
				{
					console->write("- To unpatch press \"u\" and then \"ENTER\".\n");
					console->write("- To skip just press \"ENTER\".\n");
					answer = get_input_string();
				}

				if (!answer.empty())
				{
					doom3_fix->unpatch();
					console->write("Succeeded.\n");
				}
				else
				{
					console->write("Skipped.\n");
				}

				get_input_string(press_enter_to_exit_message);
			}
			break;

		case eaxefx::Doom3FixStatus::unpatched:
			{
				auto answer = eaxefx::String{"\xFF"};

				console->write("Not patched.\n");
				console->write("\n");

				while (!(answer.empty() || answer == "p"))
				{
					console->write("- To patch press \"p\" and then \"ENTER\".\n");
					console->write("- To skip just press \"ENTER\".\n");
					answer = get_input_string();
				}

				if (!answer.empty())
				{
					doom3_fix->patch();
					console->write("Succeeded.\n");
				}
				else
				{
					console->write("Skipped.\n");
				}

				get_input_string(press_enter_to_exit_message);
			}
			break;

		case eaxefx::Doom3FixStatus::unsupported:
		default:
			console->write("Not Doom 3 or already applied a different patch.\n");
			get_input_string(press_enter_to_exit_message);
			break;
	}

	return 0;
}
catch (const std::exception& ex)
{
	if (console != nullptr)
	{
		const auto message = eaxefx::String{"[ERROR] "} + ex.what() + '\n';
		console->write_error(message);
	}

	get_input_string(press_enter_to_exit_message);

	return 1;
}
