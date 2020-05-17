The B library is a collection of data structures, algorithms, and utility
classes and functions.

The library provides the following functionality:

-   `b::arg_list`

        #include <b/arg_list.h>

    Implementation of a technique to pass a variable-length list of named
    parameters to a function or a method.

-   `b::array<T>`

        #include <b/array.h>

    Array template type. Uses a copy-on-write technique for memory
    management.

-   `b::atomic`

        #include <b/atomic.h>

    Thread-safe reference counter.

-   `b::binary_search_tree<Key_op>`

        #include <b/binary_tree.h>

    Low-level structure that implements a non-balancing binary search tree.

-   `b::cli`

        #include <b/cli.h>

    POSIX-compatible command line parser and help screen generator.

-   `b::heap<T>`

        #include <b/heap.h>

    Priority heap implementation.

-   `b::levenshtein_distance`

        #include <b/levenshtein_distance.h>

    Edit distance calculation.

-   `b::map<Key, T>`

        #include <b/map.h>

    Template implementation of an associative array container type.

-   `b::object`

        #include <b/object.h>

    Base class with reference count support.

-   `b::pathname`

        #include <b/pathname.h>

    Pathname parsing and normalization with additional modification
    operations.

-   `b::random`

        #include <b/random.h>

    A linear congruential generator.

-   `b::ref`

        #include <b/ref.h>

    Smart pointer to instances of all classes derived from 'b::object'.

-   `b::set<T>`

        #include <b/set.h>

    A set of unique objects of type T.

-   `b::string`
    `b::wstring`

        #include <b/string.h>

    Generic string type, both byte character and wide character
    versions.  Uses a copy-on-write technique.

-   `b::string_view`
    `b::wstring_view`

        #include <b/string_view.h>

    A pointer to a string segment. Useful in parameters and for
    temporary strings.

-   `b::input_stream`
    `b::output_stream`
    `b::seekable`
    `b::seekable_input_stream`
    `b::seekable_output_stream`
    `b::input_output_stream`

        #include <b/io_streams.h>

    Various input/output interfaces.

-   `b::doubly_linked_list<Node_access>`

        #include <b/doubly_linked_list.h>

    `b::doubly_linked_list_container<T>`

        #include <b/doubly_linked_list_container.h>

    `b::linked_list<Node_access>`

        #include <b/linked_list.h>

    `b::linked_list_container<T>`

        #include <b/linked_list_container.h>

    Various linked list classes.

-   `b::exception`

        #include <b/exception.h>

    `b::runtime_exception`

        #include <b/runtime_exception.h>

    `b::custom_exception`

        #include <b/custom_exception.h>

    `b::system_exception`

        #include <b/system_exception.h>

    Various exception classes.
