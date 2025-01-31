set(PROJECT_NAME Test)

project(${PROJECT_NAME})

find_package(GTest REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Matplot++ REQUIRED)
find_package(unordered_dense)

add_executable(${PROJECT_NAME}
	"TestLayerInfo.cpp" "TestContainer.cpp"
	"TestEvaluate.cpp" "TestExtract.cpp" "TestShow.cpp" "TestFilter.cpp"
	"TestTraverser.cpp" "TestUserFunc.cpp" "TestPlot.cpp" "TestMatrix.cpp")

if(${unordered_dense_FOUND})
	message("unordered_dense found")
	target_compile_definitions(${PROJECT_NAME} PRIVATE USE_ANKERL_UNORDERED_DENSE)
	target_link_libraries(${PROJECT_NAME} OpenADAPT GTest::GTest GTest::Main yaml-cpp Matplot++::matplot unordered_dense::unordered_dense)
else()
	message("unordered_dense not found")
target_link_libraries(${PROJECT_NAME} OpenADAPT GTest::GTest GTest::Main yaml-cpp Matplot++::matplot)
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

add_custom_target(COPY_FILES
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_CURRENT_SOURCE_DIR}/OpenADAPT-student_list.txt"
	"${CMAKE_CURRENT_BINARY_DIR}/")
add_dependencies(${PROJECT_NAME} COPY_FILES)


set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
		OUTPUT_NAME "OpenADAPT-${PROJECT_NAME}")

install(
	TARGETS ${PROJECT_NAME}
	EXPORT ${PACKAGE_NAME}Config
	RUNTIME DESTINATION bin/${CMAKE_BUILD_TYPE})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/OpenADAPT-student_list.txt
		DESTINATION bin/${CMAKE_BUILD_TYPE})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	install(
		DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
		DESTINATION bin/${CMAKE_BUILD_TYPE}
		FILES_MATCHING PATTERN *.pdb)
endif()