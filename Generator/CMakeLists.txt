set(PROJECT_NAME Generator)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} "Generate.cpp")

target_link_libraries(${PROJECT_NAME} OpenADAPT)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
		OUTPUT_NAME "OpenADAPT-${PROJECT_NAME}")

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	DEPENDS ${PROJECT_NAME}
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/OpenADAPT-${PROJECT_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/OpenADAPT/Evaluator)