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
#include <string>
#include <utility>
#include <vector>

#include "eaxefx_console.h"
#include "eaxefx_exception.h"
#include "eaxefx_string.h"

#include "eaxefx_patch.h"
#include "eaxefx_patch_collection.h"


class AppPatcherException :
	public eaxefx::Exception
{
public:
	explicit AppPatcherException(
		const char* message)
		:
		Exception{"APP_PATCHER", message}
	{
	}
}; // AppPatcherException


class AppPatcherCancelledException :
	public eaxefx::Exception
{
public:
	AppPatcherCancelledException()
		:
		Exception{"APP_PATCHER", "Cancelled."}
	{
	}
}; // AppPatcherCancelledException


auto console = eaxefx::ConsoleUPtr{};

static const auto press_enter_to_exit_message = "Press ENTER to exit.\n";


struct PatchStatusDef
{
	using ActionMethod = void (eaxefx::FilePatcher::*)();


	eaxefx::String name{};
	eaxefx::String answer{};
	eaxefx::String action{};
	ActionMethod action_method{};
}; // PatchStatusDef


const PatchStatusDef patch_status_defs[] =
{
	PatchStatusDef
	{
		"Unpatched",
		"p",
		"patch",
		&eaxefx::FilePatcher::apply
	},

	PatchStatusDef
	{
		"Patched",
		"u",
		"unpatch",
		&eaxefx::FilePatcher::revert
	},
};

const PatchStatusDef& get_patch_status_def(
	eaxefx::PatchStatus patch_status)
{
	switch (patch_status)
	{
		case eaxefx::PatchStatus::unpatched:
			return patch_status_defs[0];

		case eaxefx::PatchStatus::patched:
			return patch_status_defs[1];

		default:
			throw AppPatcherException{"Unsupported patch status."};
	}
}


void write_new_line()
{
	console->write("\n");
}

eaxefx::String get_input_string(
	const char* message = nullptr)
{
	if (!console)
	{
		return eaxefx::String{};
	}

	if (message)
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

	console->write("========================================================");
	write_new_line();
	console->write("EAXEFX Application Patcher v" EAXEFX_APP_PATCHER_VERSION);
	write_new_line();
	console->write("========================================================");
	write_new_line();

	const auto patch_collection = eaxefx::make_patch_collection();

	struct FoundApp
	{
		const eaxefx::Patch* patch{};
		eaxefx::FilePatcherUPtr patcher{};
	}; // FoundApp

	using FoundApps = std::vector<FoundApp>;
	auto found_apps = FoundApps{};
	found_apps.reserve(patch_collection.size());

	for (const auto& patch : patch_collection)
	{
		try
		{
			auto file_patcher = eaxefx::make_file_patcher(patch);

			if (file_patcher->get_status() != eaxefx::PatchStatus{})
			{
				found_apps.emplace_back();
				auto& found_app = found_apps.back();
				found_app.patch = &patch;
				found_app.patcher = std::move(file_patcher);
			}
		}
		catch (...)
		{
		}
	}

	if (found_apps.empty())
	{
		write_new_line();
		console->write("Not found any supported application.");
		write_new_line();

		write_new_line();
		get_input_string(press_enter_to_exit_message);

		return 1;
	}

	const auto found_app_count = static_cast<int>(found_apps.size());
	const auto is_one_app = (found_app_count == 1);

	auto app_show_index = 0;

	for (const auto& found_app : found_apps)
	{
		write_new_line();

		if (!is_one_app)
		{
			const auto index_string = eaxefx::to_string(app_show_index + 1) + ")";
			console->write(index_string);
			write_new_line();
		}

		console->write("Application: ");
		console->write(found_app.patch->name);
		write_new_line();

		const auto& patch_status_def = get_patch_status_def(found_app.patcher->get_status());
		console->write("Status: ");
		console->write(patch_status_def.name);
		write_new_line();

		console->write("Description: ");
		console->write(found_app.patch->description);
		write_new_line();

		app_show_index += 1;
	}

	write_new_line();
	console->write("--------------------------------------------------------");
	write_new_line();

	auto application_index = 0;

	if (found_app_count > 1)
	{
		write_new_line();
		console->write("To select application type in it's number and press \"ENTER\".");
		write_new_line();
		console->write("To cancel just press \"ENTER\".");
		write_new_line();
		write_new_line();

		auto application_number = 0;

		while (true)
		{
			const auto answer = get_input_string("Application number: ");

			if (answer.empty())
			{
				throw AppPatcherCancelledException{};
			}

			try
			{
				application_number = eaxefx::stoi(answer);
			}
			catch (...)
			{
			}

			if (application_number > 0 && application_number <= found_app_count)
			{
				application_index = application_number - 1;
				break;
			}
		}
	}

	auto& found_app = found_apps[application_index];
	const auto& patch_status_def = get_patch_status_def(found_app.patcher->get_status());

	write_new_line();

	if (!is_one_app)
	{
		console->write("Selected application: ");
		console->write(found_app.patch->name);
		write_new_line();
	}

	write_new_line();

	console->write("To ");
	console->write(patch_status_def.action);
	console->write(" type in `");
	console->write(patch_status_def.answer);
	console->write("` and press `ENTER`.");
	write_new_line();
	console->write("To cancel just press \"ENTER\".");
	write_new_line();
	write_new_line();

	while (true)
	{
		const auto answer = get_input_string("Action: ");

		if (answer == patch_status_def.answer)
		{
			break;
		}

		if (answer.empty())
		{
			throw AppPatcherCancelledException{};
		}
	}

	(found_app.patcher.get()->*patch_status_def.action_method)();

	write_new_line();
	console->write("Succeeded.");
	write_new_line();

	write_new_line();
	get_input_string(press_enter_to_exit_message);

	return 0;
}
catch (const AppPatcherCancelledException&)
{
	if (console)
	{
		write_new_line();
		console->write("Cancelled.");
		write_new_line();

		write_new_line();
		get_input_string(press_enter_to_exit_message);
	}

	return 1;
}
catch (const std::exception& ex)
{
	if (console)
	{
		const auto message = eaxefx::String{"[ERROR] "} + ex.what();

		write_new_line();
		console->write_error(message);
		write_new_line();
	}

	write_new_line();
	get_input_string(press_enter_to_exit_message);

	return 1;
}
