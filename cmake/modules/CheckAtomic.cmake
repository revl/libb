include(CheckCXXSourceCompiles)
include(CheckLibraryExists)

# Check for std::atomic<> in <atomic>
check_cxx_source_compiles("
int main() {
	int val = 0;
	__sync_add_and_fetch(&val, 1);
	__sync_add_and_fetch(&val, -1);
	return val;
}
" B_HAVE_ATOMIC_SYNC)
