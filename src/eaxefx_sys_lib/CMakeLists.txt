#[[

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

]]


cmake_minimum_required (VERSION 3.8.2 FATAL_ERROR)
project (eaxefx_sys_lib VERSION 1.0.2 LANGUAGES CXX)


#
# Validate target system.
#
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
option (EAXEFX_PCH "Use precompiled headers if available." ON)
message (STATUS "[${PROJECT_NAME}] Precompiled headers: ${EAXEFX_PCH}")

# Static linking.
#
option (EAXEFX_STATIC_LINKING "Use static linking." ON)
message (STATUS "[${PROJECT_NAME}] Static linking: ${EAXEFX_STATIC_LINKING}")

# All warnings.
#
option (EAXEFX_ALL_WARNINGS "Report all warnings." OFF)
message (STATUS "[${PROJECT_NAME}] Report all warnings: ${EAXEFX_ALL_WARNINGS}")


#
# Size of a void pointer.
#
message (STATUS "[${PROJECT_NAME}] Size of a void pointer: ${CMAKE_SIZEOF_VOID_P}")


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
		$<$<CXX_COMPILER_ID:MSVC>:$<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:-arch:IA32>>
		$<$<CXX_COMPILER_ID:MSVC>:$<IF:$<BOOL:${EAXEFX_ALL_WARNINGS}>,-Wall,-W4>>
		$<$<CXX_COMPILER_ID:MSVC>:-MP -GR->
		$<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:DEBUG>>:-Oy -GS->>
	PRIVATE
		$<$<CXX_COMPILER_ID:GNU>:$<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:-march=i686>>
		$<$<CXX_COMPILER_ID:GNU>:-Wpedantic>
		$<$<CXX_COMPILER_ID:GNU>:$<$<BOOL:${EAXEFX_ALL_WARNINGS}>:-Wall -Wextra>>
)

if (EAXEFX_STATIC_LINKING)
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
if (EAXEFX_PCH AND NOT (${CMAKE_VERSION} VERSION_LESS "3.16"))
	target_precompile_headers(
		${PROJECT_NAME}
		PRIVATE
			[["eaxefx_pch.h"]]
	)
endif ()


#
# Dependencies.
#
if (EAXEFX_STATIC_LINKING)
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

		include/eaxefx_c_string.h
		include/eaxefx_core_types.h
		include/eaxefx_condition_variable.h
		include/eaxefx_console.h
		include/eaxefx_encoding.h
		include/eaxefx_env.h
		include/eaxefx_exception.h
		include/eaxefx_file.h
		include/eaxefx_fs.h
		include/eaxefx_fs_path.h
		include/eaxefx_logger.h
		include/eaxefx_moveable_mutex_lock.h
		include/eaxefx_mutex.h
		include/eaxefx_platform.h
		include/eaxefx_process.h
		include/eaxefx_scope_exit.h
		include/eaxefx_shared_library.h
		include/eaxefx_span.h
		include/eaxefx_string.h
		include/eaxefx_system_time.h
		include/eaxefx_thread.h
		include/eaxefx_unit_converters.h
		include/eaxefx_rc.h

		src/eaxefx_sys_win32_critical_section.h
		src/eaxefx_sys_win32_condition_variable.h
		src/eaxefx_sys_win32_event.h
)


#
# Source files.
#
target_sources (
	${PROJECT_NAME}
	PRIVATE
		src/eaxefx_c_string.cpp
		src/eaxefx_core_types.cpp
		src/eaxefx_exception.cpp
		src/eaxefx_fs.cpp
		src/eaxefx_fs_path.cpp
		src/eaxefx_logger.cpp
		src/eaxefx_moveable_mutex_lock.cpp
		src/eaxefx_span.cpp
		src/eaxefx_string.cpp
		src/eaxefx_system_time.cpp

		src/eaxefx_win32_condition_variable.cpp
		src/eaxefx_win32_console.cpp
		src/eaxefx_win32_encoding.cpp
		src/eaxefx_win32_env.cpp
		src/eaxefx_win32_file.cpp
		src/eaxefx_win32_mutex.cpp
		src/eaxefx_win32_platform.cpp
		src/eaxefx_win32_process.cpp
		src/eaxefx_win32_shared_library.cpp
		src/eaxefx_win32_string.cpp
		src/eaxefx_win32_system_time.cpp
		src/eaxefx_win32_thread.cpp

		src/eaxefx_sys_win32_critical_section.cpp
		src/eaxefx_sys_win32_condition_variable.cpp
		src/eaxefx_sys_win32_event.cpp
)
