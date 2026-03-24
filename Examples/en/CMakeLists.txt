set(PROJECT_NAME Examples_en)

project(${PROJECT_NAME})

set(EXAMPLE_SRC "main.cpp" "quickstart_dtree.cpp" "quickstart_stree.cpp" "quickstart_dtable.cpp" "quickstart_stable.cpp"
				"quickstart_join.cpp" "ctti_rtti_typed.cpp" "quickstart_plot.cpp" "quickstart_dhist.cpp" "quickstart_shist.cpp")

if(BUILD_WITH_MODULE)
	message("Building Example/en with Module")
	set(EXAMPLES_EN_COMPILE_DEFS ADAPT_IMPORT_MODULE)
	set(EXAMPLES_EN_LINK_LIBS OpenADAPT::Module)
else()
	message("Building Example/en without Module")
	set(EXAMPLES_EN_COMPILE_DEFS)
	set(EXAMPLES_EN_LINK_LIBS OpenADAPT::OpenADAPT)
endif()

if (BUILD_PARSER)
	list(APPEND EXAMPLE_SRC quickstart_parser.cpp)
	list(APPEND EXAMPLES_EN_LINK_LIBS OpenADAPT::Parser)
endif()

add_executable(${PROJECT_NAME} ${EXAMPLE_SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE ${EXAMPLES_EN_LINK_LIBS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${EXAMPLES_EN_COMPILE_DEFS})

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