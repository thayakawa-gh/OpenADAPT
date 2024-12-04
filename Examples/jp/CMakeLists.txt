set(PROJECT_NAME Examples_jp)

project(${PROJECT_NAME})

find_package(Matplot++ REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp" "examples_plot.h")

target_link_libraries(${PROJECT_NAME} OpenADAPT Matplot++::matplot)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
		OUTPUT_NAME "OpenADAPT-${PROJECT_NAME}")

install(
	TARGETS ${PROJECT_NAME}
	EXPORT ${PACKAGE_NAME}Config
	RUNTIME DESTINATION bin/${CMAKE_BUILD_TYPE})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	install(
		DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
		DESTINATION bin/${CMAKE_BUILD_TYPE}
		FILES_MATCHING PATTERN *.pdb)
endif()