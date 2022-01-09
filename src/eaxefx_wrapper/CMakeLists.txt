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
project (eaxefx_wrapper VERSION 1.0.7 LANGUAGES CXX)


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
make_version_string (EAXEFX_VERSION)
message (STATUS "[${PROJECT_NAME}] v${EAXEFX_VERSION}")


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


#
# Size of a void pointer.
#
message (STATUS "[${PROJECT_NAME}] Size of a void pointer: ${CMAKE_SIZEOF_VOID_P}")


#
# Target.
#
add_library (${PROJECT_NAME} SHARED "")


#
# Configure files.
#
configure_file (
	"${PROJECT_SOURCE_DIR}/src/eaxefx.rc.in"
	"${PROJECT_BINARY_DIR}/src/eaxefx.rc"
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
		PREFIX ""
		OUTPUT_NAME OpenAL32
)


#
# Include directories.
#
target_include_directories (
	${PROJECT_NAME}
	PRIVATE
		lib/openal_soft/include
		include
		src
)


#
# Compile definitions.
#
target_compile_definitions (
	${PROJECT_NAME}
	PRIVATE
		"EAXEFX_VERSION=\"${EAXEFX_VERSION}\""
		_UNICODE
		UNICODE
		NOMINMAX
		WIN32_LEAN_AND_MEAN
		"AL_API=__declspec(dllexport)"
		"ALC_API=__declspec(dllexport)"
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
		$<$<CXX_COMPILER_ID:MSVC>:-W4 -MP -GR->
		$<$<CXX_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:DEBUG>>:-Oy -GS->>
	PRIVATE
		$<$<CXX_COMPILER_ID:GNU>:$<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:-march=i686>>
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

target_link_libraries (
	${PROJECT_NAME}
	PRIVATE
		eaxefx::sys_lib
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
target_sources (
	${PROJECT_NAME}
	PRIVATE
		src/eaxefx_pch.h

		include/eaxefx_common_strings.h
		include/eaxefx_main.h
		include/eaxefx_utils.h

		include/eaxefx_al_api.h
		include/eaxefx_al_api_context.h
		include/eaxefx_al_api_utils.h
		include/eaxefx_al_loader.h
		include/eaxefx_al_low_pass_param.h
		include/eaxefx_al_object.h
		include/eaxefx_al_symbols.h
		include/eaxefx_al_uresources.h

		include/eaxefx_eax_api.h

		include/eaxefx_eaxx.h
		include/eaxefx_eaxx_context.h
		include/eaxefx_eaxx_context_shared.h
		include/eaxefx_eaxx_eax_call.h
		include/eaxefx_eaxx_fx_slot.h
		include/eaxefx_eaxx_fx_slot_index.h
		include/eaxefx_eaxx_fx_slots.h
		include/eaxefx_eaxx_source.h
		include/eaxefx_eaxx_validators.h

		include/eaxefx_eaxx_effect.h
		include/eaxefx_eaxx_null_effect.h
		include/eaxefx_eaxx_auto_wah_effect.h
		include/eaxefx_eaxx_chorus_effect.h
		include/eaxefx_eaxx_compressor_effect.h
		include/eaxefx_eaxx_distortion_effect.h
		include/eaxefx_eaxx_eax_reverb_effect.h
		include/eaxefx_eaxx_echo_effect.h
		include/eaxefx_eaxx_equalizer_effect.h
		include/eaxefx_eaxx_flanger_effect.h
		include/eaxefx_eaxx_frequency_shifter_effect.h
		include/eaxefx_eaxx_pitch_shifter_effect.h
		include/eaxefx_eaxx_ring_modulator_effect.h
		include/eaxefx_eaxx_vocal_morpher_effect.h
)


#
# Source files.
#
target_sources (
	${PROJECT_NAME}
	PRIVATE
		src/eaxefx_common_strings.cpp
		src/eaxefx_main.cpp
		src/eaxefx_utils.cpp

		src/eaxefx_al_api.cpp
		src/eaxefx_al_api_context.cpp
		src/eaxefx_al_api_utils.cpp
		src/eaxefx_al_loader.cpp
		src/eaxefx_al_object.cpp
		src/eaxefx_al_symbols.cpp
		src/eaxefx_al_uresources.cpp

		src/eaxefx_eax_api.cpp

		src/eaxefx_eaxx.cpp
		src/eaxefx_eaxx_eax_call.cpp
		src/eaxefx_eaxx_context.cpp
		src/eaxefx_eaxx_context_shared.cpp
		src/eaxefx_eaxx_fx_slot.cpp
		src/eaxefx_eaxx_fx_slot_index.cpp
		src/eaxefx_eaxx_fx_slots.cpp
		src/eaxefx_eaxx_source.cpp

		src/eaxefx_eaxx_effect.cpp
		src/eaxefx_eaxx_null_effect.cpp
		src/eaxefx_eaxx_auto_wah_effect.cpp
		src/eaxefx_eaxx_chorus_effect.cpp
		src/eaxefx_eaxx_compressor_effect.cpp
		src/eaxefx_eaxx_distortion_effect.cpp
		src/eaxefx_eaxx_eax_reverb_effect.cpp
		src/eaxefx_eaxx_echo_effect.cpp
		src/eaxefx_eaxx_equalizer_effect.cpp
		src/eaxefx_eaxx_flanger_effect.cpp
		src/eaxefx_eaxx_frequency_shifter_effect.cpp
		src/eaxefx_eaxx_pitch_shifter_effect.cpp
		src/eaxefx_eaxx_ring_modulator_effect.cpp
		src/eaxefx_eaxx_vocal_morpher_effect.cpp

		"${PROJECT_BINARY_DIR}/src/eaxefx.rc"
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

install (
	FILES
		../../README.md
	DESTINATION
		"."
	RENAME
		"${PROJECT_NAME}_readme.txt"
)

install (
	FILES
		../../LICENSE.md
	DESTINATION
		"."
	RENAME
		"${PROJECT_NAME}_license.txt"
)

install (
	FILES
		../../CHANGELOG.md
	DESTINATION
		"."
	RENAME
		"${PROJECT_NAME}_changelog.txt"
)

install (
	FILES
		lib/openal_soft/COPYING
	DESTINATION
		"."
	RENAME
		"${PROJECT_NAME}_openal_soft_license.txt"
)