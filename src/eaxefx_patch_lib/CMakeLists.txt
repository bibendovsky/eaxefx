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
project (eaxefx_patch_lib VERSION 1.0.1 LANGUAGES CXX)


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
make_version_string (EAXEFX_PATCH_LIB_VERSION)
message (STATUS "[${PROJECT_NAME}] v${EAXEFX_PATCH_LIB_VERSION}")


#
# Options.
#

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
add_library (eaxefx::patch_lib ALIAS ${PROJECT_NAME})


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
		include/eaxefx_patch.h
		include/eaxefx_patch_collection.h

		include/eaxefx_file_patcher.h
		include/eaxefx_patch_validator.h
		include/eaxefx_win32_process_patcher.h
)


#
# Source files.
#
target_sources (
	${PROJECT_NAME}
	PRIVATE
		src/eaxefx_patch.cpp
		src/eaxefx_patch_collection.cpp

		src/eaxefx_file_patcher.cpp
		src/eaxefx_patch_validator.cpp
		src/eaxefx_win32_process_patcher.cpp
)
