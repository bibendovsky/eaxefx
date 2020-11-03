cmake_minimum_required (VERSION 3.8.2 FATAL_ERROR)


#
# Makes version string from project one and with optional suffix.
#
# Parameters:
#    - Result variable name.
#    - [OPTIONAL] A suffix.
#
function (make_version_string)
	if (NOT ((${ARGC} EQUAL 1) OR (${ARGC} EQUAL 2)))
		message (FATAL_ERROR "[make_version_string] Argument count out of range.")
	endif ()

	if (NOT PROJECT_VERSION)
		message (FATAL_ERROR "[make_version_string] No project version.")
	endif ()

	if (NOT ARGV1 OR ARGV1 STREQUAL "")
		set (${ARGV0} "${PROJECT_VERSION}" PARENT_SCOPE)
	else ()
		set (${ARGV0} "${PROJECT_VERSION}-${ARGV1}" PARENT_SCOPE)
	endif ()
endfunction ()
