#[[

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

]]


cmake_minimum_required (VERSION 3.8.2 FATAL_ERROR)
project (eaxefx_sys_lib VERSION 1.0.1 LANGUAGES CXX)


if (NOT WIN32)
	message (FATAL_ERROR "Unsupported platform.")
endif ()


#
# Module path.
#
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


#
# Includes.
#
include (make_version_string)


#
# Version.
#
make_version_string (EAXEFX_SYS_LIB_VERSION)
message (STATUS "[${PROJECT_NAME}] v${EAXEFX_SYS_LIB_VERSION}")


#
# Options.
#

# Precompiled headers.
#
option (EAXEFX_USE_PCH "Use precompiled headers if available." ON)
message (STATUS "[${PROJECT_NAME}] Precompiled headers: ${EAXEFX_USE_PCH}")

# Static linking.
#
option (EAXEFX_USE_STATIC_LINKING "Use static linking." ON)
message (STATUS "[${PROJECT_NAME}] Static linking: ${EAXEFX_USE_STATIC_LINKING}")


#
# Packages.
#


#
# Target.
#
add_library (${PROJECT_NAME} STATIC "")
add_library (eaxefx::sys_lib ALIAS ${PROJECT_NAME})


#
# Properties.
#
set_target_properties (
	${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
)


#
# Include directories.
#
target_include_directories (
	${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		src
)


#
# Compile definitions.
#
target_compile_definitions (
	${PROJECT_NAME}
	PRIVATE
		_UNICODE
		UNICODE
		NOMINMAX
		WIN32_LEAN_AND_MEAN
		$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
		$<$<CXX_COMPILER_ID:MSVC>:_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS>
)


#
# Compile options.
#
target_compile_options (
	${PROJECT_NAME}
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:-W4 -MP -arch:IA32 -GR->
		$<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:DEBUG>>:-Oy -GS->>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

if (EAXEFX_USE_STATIC_LINKING)
	if (MSVC)
		target_compile_options (
			${PROJECT_NAME}
			PRIVATE
				$<$<CONFIG:DEBUG>:-MTd>
				$<$<NOT:$<CONFIG:DEBUG>>:-MT>
		)
	endif ()
endif ()


#
# Precompile headers.
#
if (EAXEFX_USE_PCH AND NOT (${CMAKE_VERSION} VERSION_LESS "3.16"))
	target_precompile_headers(
		${PROJECT_NAME}
		PRIVATE
			[["eaxefx_pch.h"]]
	)
endif ()


#
# Dependencies.
#
if (EAXEFX_USE_STATIC_LINKING)
	if (NOT MSVC)
		target_link_libraries (
			${PROJECT_NAME}
			PRIVATE
				-static
		)
	endif ()
endif ()


#
# Header files.
#
target_sources (
	${PROJECT_NAME}
	PRIVATE
		src/eaxefx_pch.h

		include/eaxefx_condition_variable.h
		include/eaxefx_console.h
		include/eaxefx_encoding.h
		include/eaxefx_exception.h
		include/eaxefx_file.h
		include/eaxefx_file_system.h
		include/eaxefx_logger.h
		include/eaxefx_mutex.h
		include/eaxefx_not_null.h
		include/eaxefx_platform.h
		include/eaxefx_process.h
		include/eaxefx_scope_exit.h
		include/eaxefx_shared_library.h
		include/eaxefx_string.h
		include/eaxefx_system_time.h
		include/eaxefx_thread.h
		include/eaxefx_unit_converters.h
		include/eaxefx_rc.h

		src/eaxefx_win32_condition_variable.h
		src/eaxefx_win32_critical_section.h
		src/eaxefx_win32_event.h
)


#
# Source files.
#
target_sources (
	${PROJECT_NAME}
	PRIVATE
		src/eaxefx_condition_variable.cpp
		src/eaxefx_console.cpp
		src/eaxefx_encoding.cpp
		src/eaxefx_exception.cpp
		src/eaxefx_file.cpp
		src/eaxefx_file_system.cpp
		src/eaxefx_logger.cpp
		src/eaxefx_mutex.cpp
		src/eaxefx_platform.cpp
		src/eaxefx_process.cpp
		src/eaxefx_shared_library.cpp
		src/eaxefx_string.cpp
		src/eaxefx_system_time.cpp
		src/eaxefx_thread.cpp

		src/eaxefx_win32_condition_variable.cpp
		src/eaxefx_win32_critical_section.cpp
		src/eaxefx_win32_event.cpp
)
