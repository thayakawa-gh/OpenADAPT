cmake_minimum_required(VERSION 3.29)

project(Module)

add_library(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if(${unordered_dense_FOUND})
	message("unordered_dense found")
	target_compile_definitions(${PROJECT_NAME} PRIVATE ADAPT_USE_ANKERL_UNORDERED_DENSE)
	target_link_libraries(${PROJECT_NAME} PUBLIC OpenADAPT unordered_dense::unordered_dense)
else()
	message("unordered_dense not found")
	target_link_libraries(${PROJECT_NAME} PUBLIC OpenADAPT)
endif()

target_sources(${PROJECT_NAME} PUBLIC
	FILE_SET modules TYPE CXX_MODULES FILES "ADAPT.ixx")
add_library(${PACKAGE_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

install(
	TARGETS ${PROJECT_NAME}
	EXPORT ${PACKAGE_NAME}Config
	RUNTIME DESTINATION bin/${CMAKE_BUILD_TYPE}
	LIBRARY DESTINATION lib/${CMAKE_BUILD_TYPE}
	ARCHIVE DESTINATION lib/${CMAKE_BUILD_TYPE}
	CXX_MODULES_BMI DESTINATION module/${CMAKE_BUILD_TYPE}
	INCLUDES DESTINATION include/${PACKAGE_NAME}
	FILE_SET modules DESTINATION include/${PACKAGE_NAME}/${PROJECT_NAME})
