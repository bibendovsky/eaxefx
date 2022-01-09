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
project (eaxefx_app_patcher VERSION 1.0.0 LANGUAGES CXX)


#
# Validate target system.
#
if (NOT WIN32)
	message (FATAL_ERROR "Unsupported platform.")
endif ()

message (STATUS "[${PROJECT_NAME}] Size of a void pointer: ${CMAKE_SIZEOF_VOID_P}")

set (EAXEFX_COMPILER_ARCH_OPTION "")

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
	if (MSVC)
		set (EAXEFX_COMPILER_ARCH_OPTION "-arch:IA32")
	endif ()
elseif (CMAKE_SIZEOF_VOID_P EQUAL 8)
else ()
	message (FATAL_ERROR "[${PROJECT_NAME}] Unsupported CPU bit count.")
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
make_version_string (EAXEFX_APP_PATCHER_VERSION)
message (STATUS "[${PROJECT_NAME}] v${EAXEFX_APP_PATCHER_VERSION}")


#
# Options.
#

# Static linking.
#
option (EAXEFX_STATIC_LINKING "Use static linking." ON)
message (STATUS "[${PROJECT_NAME}] Static linking: ${EAXEFX_STATIC_LINKING}")


#
# Packages.
#


#
# Target.
#
add_executable (${PROJECT_NAME} "")


#
# Configure files.
#
configure_file (
	"${PROJECT_SOURCE_DIR}/src/eaxefx_app_patcher.rc.in"
	"${PROJECT_BINARY_DIR}/src/eaxefx_app_patcher.rc"
)


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


#
# Compile definitions.
#
target_compile_definitions (
	${PROJECT_NAME}
	PRIVATE
		"EAXEFX_APP_PATCHER_VERSION=\"${EAXEFX_APP_PATCHER_VERSION}\""
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
		$<$<CXX_COMPILER_ID:MSVC>:-W4 -MP ${EAXEFX_COMPILER_ARCH_OPTION} -GR->
		$<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:DEBUG>>:-Oy -GS->>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
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
	PRIVATE
		eaxefx::patch_lib
)


#
# Custom commands.
#
if (CMAKE_STRIP AND CMAKE_BUILD_TYPE STREQUAL Release)
	add_custom_command (
		TARGET
			${PROJECT_NAME}
		POST_BUILD
			COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>
	)
endif ()


#
# Header files.
#


#
# Source files.
#
target_sources (
	${PROJECT_NAME}
	PRIVATE
		src/eaxefx_app_patcher.cpp
		"${PROJECT_BINARY_DIR}/src/eaxefx_app_patcher.rc"
)


#
# Install.
#
install (
	TARGETS
		${PROJECT_NAME}
	RUNTIME DESTINATION
		"."
)
