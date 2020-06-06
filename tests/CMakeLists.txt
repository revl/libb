set(UNIT_TESTS
	arg_list_test
	array_slice_test
	array_test
	atomic_test
	binary_search_tree_test
	cli_test
	exceptions_test
	fn_test
	heap_test
	io_stream_test
	levenshtein_distance_test
	lists_test
	map_test
	memory_test
	object_test
	opaque_test
	pathname_test
	priority_queue_test
	pseudorandom_test
	red_black_tree_test
	set_test
	string_formatting_test
	string_stream_test
	string_test
	string_view_test
)

foreach(TEST_NAME IN LISTS UNIT_TESTS)
	add_executable(${TEST_NAME} ${TEST_NAME}.cc)
	target_link_libraries(${TEST_NAME} ${PROJECT_NAME})
	add_test(${TEST_NAME} ${TEST_NAME})
endforeach(TEST_NAME)

if(BUILD_COVERAGE)
	setup_target_for_coverage_lcov(
		NAME coverage
		EXECUTABLE ctest -j ${PROCESSOR_COUNT}
		DEPENDENCIES ${UNIT_TESTS}
		EXCLUDE "/usr/*"
			"${PROJECT_SOURCE_DIR}/tests/test_case.h"
	)
endif()
