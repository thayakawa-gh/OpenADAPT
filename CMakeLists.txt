cmake_minimum_required(VERSION 3.8)

set(PACKAGE_NAME OpenADAPT)
set(VERSION 2.0.0-alpha0)

project(${PACKAGE_NAME})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/W4 /utf-8 /bigobj /D_CRT_SECURE_NO_WARNINGS /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
else()
	add_compile_options(-Wall -Wextra)
endif()

add_library(${PACKAGE_NAME} INTERFACE)
add_library(${PACKAGE_NAME}::${PACKAGE_NAME} ALIAS ${PACKAGE_NAME})

target_include_directories(${PACKAGE_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
install(
	TARGETS ${PACKAGE_NAME}
	EXPORT ${PACKAGE_NAME}Config)

option(GENERATE_CODE "generate code" OFF)
option(BUILD_TEST "build test program" OFF)
option(BUILD_EXAMPLES "build examples" OFF)

if(GENERATE_CODE)
	add_subdirectory(Generator)
endif()
if(BUILD_TEST)
	add_subdirectory(Test)
endif()
if(BUILD_EXAMPLES)
	add_subdirectory(Examples)
endif()

install(
	EXPORT ${PACKAGE_NAME}Config
	DESTINATION cmake
	NAMESPACE ${PACKAGE_NAME}::)
	
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
	VERSION ${VERSION}
	COMPATIBILITY ExactVersion)

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake"
	DESTINATION "cmake")

install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}
	DESTINATION include/${PROJECT_NAME}
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.natvis")
