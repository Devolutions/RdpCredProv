cmake_minimum_required(VERSION 3.16)

project(RdpCredProv C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CMakePackageConfigHelpers)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION_STRINGS)
list(GET VERSION_STRINGS 0 VERSION_STRING)

string(TIMESTAMP CURRENT_YEAR "%Y")
set(RDPCREDPROV_VENDOR "Devolutions Inc.")
set(RDPCREDPROV_COPYRIGHT "Copyright 2013-${CURRENT_YEAR} ${RDPCREDPROV_VENDOR}")

# Enforce strict 4-part version string: MAJOR.MINOR.PATCH.BUILD
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$" _ ${VERSION_STRING})
if(NOT CMAKE_MATCH_COUNT EQUAL 4)
    message(FATAL_ERROR "VERSION_STRING must be in the format MAJOR.MINOR.PATCH.BUILD (e.g., 1.0.0.0)")
endif()

set(RDPCREDPROV_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(RDPCREDPROV_VERSION_MINOR "${CMAKE_MATCH_2}")
set(RDPCREDPROV_VERSION_PATCH "${CMAKE_MATCH_3}")
set(RDPCREDPROV_VERSION_BUILD "${CMAKE_MATCH_4}")

set(RDPCREDPROV_VERSION "${RDPCREDPROV_VERSION_MAJOR}.${RDPCREDPROV_VERSION_MINOR}.${RDPCREDPROV_VERSION_PATCH}.${RDPCREDPROV_VERSION_BUILD}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
	include(WindowsRC)
	include(MSVCRuntime)

	if(NOT DEFINED MSVC_RUNTIME)
		set(MSVC_RUNTIME "static")
	endif()

	configure_msvc_runtime()
endif()

if(WIN32)
	set(C_FLAGS "")
	set(C_FLAGS "${C_FLAGS} -D_UNICODE")
	set(C_FLAGS "${C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
	set(C_FLAGS "${C_FLAGS} -DWIN32_LEAN_AND_MEAN")
	set(C_FLAGS "${C_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
	set(C_FLAGS "${C_FLAGS} -DWINVER=0x0602 -D_WIN32_WINNT=0x0602")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS}")
endif()

add_subdirectory(RdpCredProv)
