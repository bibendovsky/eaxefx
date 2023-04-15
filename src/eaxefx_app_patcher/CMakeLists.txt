#[[
EAX App Patcher
Copyright (c) 2020-2023 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.
SPDX-License-Identifier: MIT
]]

cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)
project(eaxefx_app_patcher VERSION 1.0.5 LANGUAGES C)

set(EAXEFX_APP_PATCHER_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
message(STATUS "[${PROJECT_NAME}] v${EAXEFX_APP_PATCHER_VERSION}")

option(EAXEFX_STATIC_LINKING "Use static linking." ON)
message(STATUS "[${PROJECT_NAME}] Static linking: ${EAXEFX_STATIC_LINKING}")

option(EAXEFX_ALL_WARNINGS "Report all warnings." OFF)
message(STATUS "[${PROJECT_NAME}] Report all warnings: ${EAXEFX_ALL_WARNINGS}")

option(EAXEFX_APP_PATCHER_CUSTOM_CRT "Use custom CRT." ON)
message(STATUS "[${PROJECT_NAME}] Use custom CRT: ${EAXEFX_APP_PATCHER_CUSTOM_CRT}")

unset(EAXEFX_APP_PATCHER_CUSTOM_CRT_ENABLED)

if (EAXEFX_APP_PATCHER_CUSTOM_CRT)
	if (MSVC)
		set(EAXEFX_APP_PATCHER_CUSTOM_CRT_ENABLED "1")
	endif ()
endif ()

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

if (EAXEFX_APP_PATCHER_CUSTOM_CRT_ENABLED)
	target_compile_definitions(
		${PROJECT_NAME}
		PRIVATE
			EAXEFX_APP_PATCHER_CUSTOM_CRT
	)

	if (MSVC)
		string(APPEND CMAKE_EXE_LINKER_FLAGS " /NODEFAULTLIB /ENTRY:main__")
		string(REGEX REPLACE "/RTC[0-9a-zA-Z]+" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
	endif ()
endif ()

target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		"EAXEFX_APP_PATCHER_VERSION=\"${EAXEFX_APP_PATCHER_VERSION}\""
		$<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

target_compile_options(
	${PROJECT_NAME}
	PRIVATE
		$<$<C_COMPILER_ID:MSVC>:$<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:-arch:IA32>>
		$<$<C_COMPILER_ID:MSVC>:$<IF:$<BOOL:${EAXEFX_ALL_WARNINGS}>,-Wall,-W4>>
		$<$<C_COMPILER_ID:MSVC>:/GR->
		$<$<C_COMPILER_ID:MSVC>:$<$<NOT:$<CONFIG:DEBUG>>:/Oy>>
		$<$<C_COMPILER_ID:MSVC>:$<$<OR:$<NOT:$<CONFIG:DEBUG>>,$<BOOL:${EAXEFX_APP_PATCHER_CUSTOM_CRT}>>:/GS->>
	PRIVATE
		$<$<C_COMPILER_ID:GNU>:$<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:-march=i686>>
		$<$<C_COMPILER_ID:GNU>:-Wpedantic>
		$<$<C_COMPILER_ID:GNU>:$<$<BOOL:${EAXEFX_ALL_WARNINGS}>:-Wall -Wextra>>
)

if (EAXEFX_STATIC_LINKING)
	if (MSVC)
		target_compile_options(
			${PROJECT_NAME}
			PRIVATE
				$<$<CONFIG:DEBUG>:-MTd>
				$<$<NOT:$<CONFIG:DEBUG>>:-MT>
		)
	else ()
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
	TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION "."
)

install(
	FILES README.md
	DESTINATION "."
	RENAME "${PROJECT_NAME}.txt"
)

install(
	FILES CHANGELOG.md
	DESTINATION "."
	RENAME "${PROJECT_NAME}_changelog.txt"
)
