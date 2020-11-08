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
project (eaxefx_doom3_fix_lib VERSION 1.0.1 LANGUAGES CXX)


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
make_version_string (EAXEFX_DOOM3_FIX_LIB_VERSION)
message (STATUS "[${PROJECT_NAME}] v${EAXEFX_DOOM3_FIX_LIB_VERSION}")


#
# Options.
#

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
add_library (eaxefx::doom3_fix_lib ALIAS ${PROJECT_NAME})


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

target_link_libraries (
	${PROJECT_NAME}
	PUBLIC
		eaxefx::sys_lib
)


#
# Header files.
#
target_sources (
	${PROJECT_NAME}
	PRIVATE
		include/eaxefx_doom3_fix_lib.h
)


#
# Source files.
#
target_sources (
	${PROJECT_NAME}
	PRIVATE
		src/eaxefx_doom3_fix_lib.cpp
)
