project(Parser)

set(PARSER_TYPES DTree DTable)
set(PARSER_SUBJECTS unary_ops binary_ops
	1arg_funcs 2arg_funcs 3arg_funcs
	1arg_lfuncs 2arg_lfuncs
	1arg_cmeths 2arg_cmeths
	1arg_fmeths 2arg_fmeths 3arg_fmeths)

#まずinterfaceとしてのParserを用意。
add_library(Parser INTERFACE)
add_library(${PACKAGE_NAME}::Parser ALIAS Parser)

message("Building Parser")
set(LIBS)
foreach(TYPE ${PARSER_TYPES})
	#ループ内で各コンテナ型に対応するParser_${TYPE}.libを作成。
	set(PARSER_SOURCES)
	foreach(SUBJ ${PARSER_SUBJECTS})
			configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/Parser_impl_${SUBJ}.cpp.in
			${CMAKE_CURRENT_BINARY_DIR}/Parser_impl_${TYPE}_${SUBJ}.cpp)
		list(APPEND PARSER_SOURCES
			${CMAKE_CURRENT_BINARY_DIR}/Parser_impl_${TYPE}_${SUBJ}.cpp)
	endforeach()
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp.in
		${CMAKE_CURRENT_BINARY_DIR}/Parser_${TYPE}.cpp)
	list(APPEND PARSER_SOURCES
		${CMAKE_CURRENT_BINARY_DIR}/Parser_${TYPE}.cpp)
	add_library(Parser_${TYPE} STATIC ${PARSER_SOURCES})
	target_include_directories(${PROJECT_NAME}_${TYPE} INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
		$<INSTALL_INTERFACE:include/${PACKAGE_NAME}>)
	target_link_libraries(${PROJECT_NAME}_${TYPE} PRIVATE OpenADAPT::OpenADAPT)
	target_compile_features(${PROJECT_NAME}_${TYPE} PRIVATE cxx_std_20)
	list(APPEND LIBS ${PROJECT_NAME}_${TYPE})
	install(
		TARGETS ${PROJECT_NAME}_${TYPE}
		EXPORT ${PACKAGE_NAME}Config
		ARCHIVE DESTINATION lib)
endforeach()

#Parserインターフェースライブラリに、全てのParser_${TYPE}ライブラリをリンク。
target_link_libraries(Parser INTERFACE ${LIBS})
message("LIBS = ${LIBS}")

install(
	TARGETS ${PROJECT_NAME}
	EXPORT ${PACKAGE_NAME}Config)