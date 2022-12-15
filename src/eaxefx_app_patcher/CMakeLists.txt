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

cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)
project(eaxefx_app_patcher VERSION 1.0.3 LANGUAGES NONE)

set(EAXEFX_APP_PATCHER_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
message(STATUS "[${PROJECT_NAME}] v${EAXEFX_APP_PATCHER_VERSION}")

option(EAXEFX_STATIC_LINKING "Use static linking." ON)
message(STATUS "[${PROJECT_NAME}] Static linking: ${EAXEFX_STATIC_LINKING}")

option(EAXEFX_ALL_WARNINGS "Report all warnings." OFF)
message(STATUS "[${PROJECT_NAME}] Report all warnings: ${EAXEFX_ALL_WARNINGS}")

message(STATUS "[${PROJECT_NAME}] Size of a void pointer: ${CMAKE_SIZEOF_VOID_P}")

option(EAXEFX_PREFER_LCC_WIN "Prefer lcc-win environment." ON)
set(EAXEFX_LCC_WIN_DIR "" CACHE PATH "Directory with lcc-win.")

unset(EAXEFX_LCC_WIN_DETECTED)

if (EAXEFX_PREFER_LCC_WIN)
	if (WIN32 AND EAXEFX_LCC_WIN_DIR)
		set(EAXEFX_LCC_WIN_LC "${EAXEFX_LCC_WIN_DIR}/bin/lc.exe")
		set(EAXEFX_LCC_WIN_LCC "${EAXEFX_LCC_WIN_DIR}/bin/lcc.exe")
		set(EAXEFX_LCC_WIN_LCCLNK "${EAXEFX_LCC_WIN_DIR}/bin/lcclnk.exe")

		if (EXISTS "${EAXEFX_LCC_WIN_LC}" AND EXISTS "${EAXEFX_LCC_WIN_LCC}" AND EXISTS "${EAXEFX_LCC_WIN_LCCLNK}")
			set(EAXEFX_LCC_WIN_DETECTED "1")
			message(STATUS "[${PROJECT_NAME}] Found lcc-win: ${EAXEFX_LCC_WIN_DIR}")
		endif ()
	endif ()
endif ()

if (EAXEFX_LCC_WIN_DETECTED)
	configure_file(
		"${PROJECT_SOURCE_DIR}/src/eaxefx_app_patcher.rc.in"
		"${PROJECT_BINARY_DIR}/src/eaxefx_app_patcher.rc"
	)

	file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/src" EAXEFX_APP_PATCHER_INC_DIR_W)
	set(EAXEFX_APP_PATCHER_INC_DIR_OPT "-I${EAXEFX_APP_PATCHER_INC_DIR_W}")

	set(EAXEFX_APP_PATCHER_C "${PROJECT_SOURCE_DIR}/src/eaxefx_app_patcher.c")
	file(TO_NATIVE_PATH ${EAXEFX_APP_PATCHER_C} EAXEFX_APP_PATCHER_C_W)

	set(EAXEFX_APP_PATCHER_RC "${PROJECT_BINARY_DIR}/src/eaxefx_app_patcher.rc")
	file(TO_NATIVE_PATH ${EAXEFX_APP_PATCHER_RC} EAXEFX_APP_PATCHER_RC_W)

	set(EAXEFX_APP_PATCHER_EXE "${PROJECT_BINARY_DIR}/src/eaxefx_app_patcher.exe")
	file(TO_NATIVE_PATH ${EAXEFX_APP_PATCHER_EXE} EAXEFX_APP_PATCHER_EXE_W)

	set(EAXEFX_APP_PATCHER_DEFINE_VERSION "-DEAXEFX_APP_PATCHER_VERSION=\"${EAXEFX_APP_PATCHER_VERSION}\"")

	add_custom_command(
		OUTPUT ${EAXEFX_APP_PATCHER_EXE}
		COMMAND ${EAXEFX_LCC_WIN_LC}
			-ansic
			-O
			-p6=no
			-unused
			-DNDEBUG
			-DEAXEFX_APP_PATCHER_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
			-DEAXEFX_APP_PATCHER_VERSION_MINOR=${PROJECT_VERSION_MINOR}
			-DEAXEFX_APP_PATCHER_VERSION_PATCH=${PROJECT_VERSION_PATCH}
			${EAXEFX_APP_PATCHER_INC_DIR_OPT}
			${EAXEFX_APP_PATCHER_C_W}
			${EAXEFX_APP_PATCHER_RC_W}
			-subsystem console
			-nolibc -s
			-o ${EAXEFX_APP_PATCHER_EXE_W}
		VERBATIM
	)

	add_custom_target(${PROJECT_NAME} ALL
		DEPENDS "${EAXEFX_APP_PATCHER_EXE}"
		VERBATIM
	)

	install(
		PROGRAMS ${EAXEFX_APP_PATCHER_EXE}
		DESTINATION ${CMAKE_INSTALL_PREFIX}
	)
else ()
	enable_language(C)

	add_executable(${PROJECT_NAME} "")

	configure_file(
		"${PROJECT_SOURCE_DIR}/src/eaxefx_app_patcher.rc.in"
		"${PROJECT_BINARY_DIR}/src/eaxefx_app_patcher.rc"
	)

	target_include_directories(
		${PROJECT_NAME}
		PRIVATE
			src
	)

	target_compile_definitions(
		${PROJECT_NAME}
		PRIVATE
			"EAXEFX_APP_PATCHER_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
			"EAXEFX_APP_PATCHER_VERSION_MINOR=${PROJECT_VERSION_MINOR}"
			"EAXEFX_APP_PATCHER_VERSION_PATCH=${PROJECT_VERSION_PATCH}"
			$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
	)

	target_compile_options(
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
			target_compile_options(
				${PROJECT_NAME}
				PRIVATE
					$<$<CONFIG:DEBUG>:-MTd>
					$<$<NOT:$<CONFIG:DEBUG>>:-MT>
			)
		endif ()
	endif ()

	if (EAXEFX_STATIC_LINKING)
		if (NOT MSVC)
			target_link_libraries(
				${PROJECT_NAME}
				PRIVATE
					-static
			)
		endif ()
	endif ()

	if (CMAKE_STRIP AND CMAKE_BUILD_TYPE STREQUAL Release)
		add_custom_command(
			TARGET
				${PROJECT_NAME}
			POST_BUILD
				COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>
		)
	endif ()

	target_sources(
		${PROJECT_NAME}
		PRIVATE
			src/eaxefx_app_patcher.c
			"${PROJECT_BINARY_DIR}/src/eaxefx_app_patcher.rc"
	)

	install(
		TARGETS
			${PROJECT_NAME}
		RUNTIME DESTINATION
			"."
	)
endif ()
