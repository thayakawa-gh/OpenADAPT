cmake_minimum_required(VERSION 3.28)

set(PACKAGE_NAME OpenADAPT)
set(VERSION 2.0.0-alpha0)

project(${PACKAGE_NAME}Package)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/W4 /utf-8 /bigobj /D_CRT_SECURE_NO_WARNINGS)
else()
	add_compile_options(-Wall -Wextra)
endif()

install(
	EXPORT ${PACKAGE_NAME}Config
	DESTINATION cmake
	NAMESPACE ${PACKAGE_NAME}::)

option(ENABLE_MODULE "enable C++ module" OFF)
option(ENABLE_PREBUILT_PARSER "enable lambda function parser" OFF)
option(PREBUILT_PARSER_TARGET "target container list for prebuilt parser (semicolon-separated list of DTree, DTable, DHist)" "")
option(BUILD_TEST "build test program" OFF)
option(BUILD_EXAMPLES "build examples" OFF)

add_subdirectory(${PACKAGE_NAME})
set(BUILD_PARSER OFF)
set(BUILD_PARSER_WITH "DTree;DTable;DHist")
if(ENABLE_PREBUILT_PARSER)
	set(BUILD_PARSER ON)
	if (PREBUILT_PARSER_TARGET) 
		set(BUILD_PARSER_WITH ${PREBUILT_PARSER_TARGET})
	endif()
	add_subdirectory(PrebuiltParser)
endif()
set(BUILD_WITH_MODULE OFF)
if(ENABLE_MODULE)
	set(BUILD_WITH_MODULE ON)
	add_subdirectory(Module)
endif()
if(BUILD_TEST)
	add_subdirectory(Test)
endif()
if(BUILD_EXAMPLES)
	add_subdirectory(Examples/jp)
	add_subdirectory(Examples/en)
	add_subdirectory(Examples/Patterns)
endif()
	
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
	VERSION ${VERSION}
	COMPATIBILITY ExactVersion)

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake"
	DESTINATION "cmake")