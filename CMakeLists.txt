cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 98)

project(b VERSION 0.1.0 LANGUAGES CXX)

string(APPEND CMAKE_CXX_FLAGS_DEBUG " -DB_DEBUG")

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

target_include_directories(${PROJECT_NAME} PUBLIC
	${PROJECT_SOURCE_DIR}/include)

configure_file(cmake/config.h.in include/${PROJECT_NAME}/config.h)
target_include_directories(${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	$<INSTALL_INTERFACE:include>
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/config.h"
	DESTINATION include/${PROJECT_NAME}
)