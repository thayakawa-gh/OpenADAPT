set(PROJECT_NAME Examples_patterns)

project(${PROJECT_NAME})

if(BUILD_WITH_MODULE)
	message("Building Examples/Patterns with Module")
	set(EXAMPLES_PATTERNS_COMPILE_DEFS ADAPT_IMPORT_MODULE)
	set(EXAMPLES_PATTERNS_LINK_LIBS OpenADAPT::Module)
else()
	message("Building Examples/Patterns without Module")
	set(EXAMPLES_PATTERNS_COMPILE_DEFS)
	set(EXAMPLES_PATTERNS_LINK_LIBS OpenADAPT::OpenADAPT)
endif()

add_executable(${PROJECT_NAME} "main.cpp")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE ${EXAMPLES_PATTERNS_LINK_LIBS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${EXAMPLES_PATTERNS_COMPILE_DEFS})

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
