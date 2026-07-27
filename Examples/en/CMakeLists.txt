set(PROJECT_NAME Examples_en)

project(${PROJECT_NAME})

find_package(rapidjson)

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
	list(APPEND EXAMPLES_EN_LINK_LIBS OpenADAPT::Parser)
	list(APPEND EXAMPLES_EN_COMPILE_DEFS ADAPT_ENABLE_PREBUILT_PARSER)
endif()

if(${rapidjson_FOUND})
	message("rapidjson found")
	list(APPEND EXAMPLES_EN_LINK_LIBS rapidjson)
	list(APPEND EXAMPLES_EN_COMPILE_DEFS ADAPT_USE_RAPIDJSON)
else()
	message("rapidjson not found")
endif()

add_executable(${PROJECT_NAME} "main.cpp" "quickstart_dtree.cpp" "quickstart_stree.cpp" "quickstart_dtable.cpp" "quickstart_stable.cpp"
			   "quickstart_join.cpp" "ctti_rtti_typed.cpp" "quickstart_plot.cpp" "quickstart_dhist.cpp" "quickstart_shist.cpp" 
			   "quickstart_parser.cpp" "quickstart_fileio.cpp")
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