cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 98)

project(b VERSION 0.1.0 LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Werror)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-Wpedantic COMPILER_SUPPORTS_WPEDANTIC)
if(COMPILER_SUPPORTS_WPEDANTIC)
	add_compile_options(-Wpedantic)
endif()

list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake/Modules)

include(CheckAtomicIncDec)

include(CheckTypeSize)
CHECK_TYPE_SIZE("size_t" B_SIZEOF_SIZE_T LANGUAGE CXX)

include(TestForSTDNamespace)
if(NOT CMAKE_NO_STD_NAMESPACE)
	option(B_USE_STL "Enable STL support" ON)
endif()

add_library(${PROJECT_NAME}
	src/binary_search_tree.cc
	src/cli.cc
	src/exceptions.cc
	src/fn.cc
	src/io_streams.cc
	src/memory.cc
	src/object.cc
	src/pathname.cc
	src/red_black_tree.cc
	src/string.cc
	src/string_stream.cc
)

target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:B_DEBUG>)

set(CONFIG_H include/${PROJECT_NAME}/config.h)

configure_file(cmake/config.h.in ${CONFIG_H})

target_include_directories(${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
	set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME B)

	include(GNUInstallDirs)

	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_H}
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
	)

	install(TARGETS ${PROJECT_NAME}
		EXPORT BTargets
		DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)

	install(EXPORT BTargets
		FILE BTargets.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/B
	)

	include(CMakePackageConfigHelpers)
	write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/BConfigVersion.cmake
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion
	)

	configure_package_config_file(
		${PROJECT_SOURCE_DIR}/cmake/BConfig.cmake.in
		${CMAKE_CURRENT_BINARY_DIR}/BConfig.cmake
		INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/B
	)

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/BConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/BConfigVersion.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/B
	)

	export(EXPORT BTargets
		FILE ${CMAKE_CURRENT_BINARY_DIR}/BTargets.cmake
	)

	export(PACKAGE B)

	include(CTest)

	if(BUILD_TESTING)
		option(BUILD_COVERAGE "Enable code coverage report" OFF)

		if(BUILD_COVERAGE)
			include(CodeCoverage)

			append_coverage_compiler_flags()
		endif()

		add_subdirectory(tests)
	endif()

	include(InstallRequiredSystemLibraries)
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
	set(CPACK_PACKAGE_VERSION_MAJOR "${b_VERSION_MAJOR}")
	set(CPACK_PACKAGE_VERSION_MINOR "${b_VERSION_MINOR}")
	include(CPack)
endif()
