The B library is a collection of data structures, algorithms, and utility
classes and functions.

The library provides the following functionality:

    #include <b/arg_list.h>                 b::arg_list

Implementation of an obscure technique for passing a variable-length
list of named parameters to a function or a method.


    #include <b/array.h>                    b::array<T>

Array template type. Uses a copy-on-write technique for memory
management.


    #include <b/atomic.h>                   b::atomic

Portable thread-safe reference counter.


    #include <b/binary_tree.h>              b::binary_search_tree<Key_op>

Low-level structure that implements a non-balancing binary search tree.


    #include <b/cli.h>                      b::cli

POSIX-compatible command line parser and help screen generator.


    #include <b/heap.h>                     b::heap<T>

Priority heap implementation.


    #include <b/levenshtein_distance.h>     b::levenshtein_distance

Edit distance calculation.


    #include <b/map.h>                      b::map<Key, T>

Template implementation of an associative array container type.


    #include <b/object.h>                   b::object

Base class with reference count support.


    #include <b/pathname.h>                 b::pathname

Pathname parsing and normalization with additional modification
operations.


    #include <b/random.h>                   b::random

A linear congruential generator.


    #include <b/ref.h>                      b::ref

Smart pointer to instances of all classes derived from 'b::object'.


    #include <b/set.h>                      b::set<T>

A set of unique objects of type T.


    #include <b/string.h>                   b::string
                                            b::wstring

Generic string type, both byte character and wide character
versions.  Uses a copy-on-write technique.


    #include <b/string_view.h>              b::string_view
                                            b::wstring_view

A pointer to a string segment. Useful in parameters and for
temporary strings.


    #include <b/io_streams.h>               b::input_stream
                                            b::output_stream
                                            b::seekable
                                            b::seekable_input_stream
                                            b::seekable_output_stream
                                            b::input_output_stream

Various input/output interfaces.


    #include <b/doubly_linked_list.h>       b::doubly_linked_list<Node_access>
    #include <b/doubly_linked_list_container.h>
                                            b::doubly_linked_list_container<T>
    #include <b/linked_list.h>              b::linked_list<Node_access>
    #include <b/linked_list_container.h>    b::linked_list_container<T>

Various linked list classes.


    #include <b/exception.h>                b::exception
    #include <b/runtime_exception.h>        b::runtime_exception
    #include <b/custom_exception.h>         b::custom_exception
    #include <b/system_exception.h>         b::system_exception

Various exception classes.
