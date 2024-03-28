# ypython
A single header version of python for c.

> 'y' here means 'yingshaoxo'. The full name should be "yingshaoxo's python".

> It should have all python built_in type and functions, for example: https://docs.python.org/3/library/functions.html

## How to use
Just import it like:
```c
#include "./y_python.h"
#include "./y_python_linux.h"
```

## Ideas
### How to use python to write a c dependencies manager or compiler?

They all require the c coder use relative header importing like `#include "./folder_path/*.h"`

#### Method 1, require every function and variable have unique names, and code is written in .h file, will produce a single ".h file"
1. You have to do a parse on the main.c file, so that you could get the `#include "<.h_file_path>"`
2. Then you do parse again on the .h_file_path to get more ".h header file paths", the final data you get is like a node tree, for each folder level, you have a lot of .h files.
3. put all code from bottom(deepest) to top, into a single .h file, then use gcc to do the compile.

#### Method 2, require every function and variable have unique names, will produce a single ".h file"
1. You have to do a parse on the main.c file, so that you could get the `#include "<.h_file_path>"`, then inside of that .h file folder, you do a search for the same file name .c file, you put the .c file code at the bottom of the .h file.
2. Then you do parse again on the .h_file_path to get more ".h header file paths", the final data you get is like a node tree, for each folder level, you have a lot of .h files.
3. put all code from bottom(deepest in tree) to top, into a single .h file, then use gcc to do the compile.

## How to improve the current c?
1. Make a built_in garbage collector.
2. Make a built_in auto_string, auto_int, auto_float, auto_list, auto_dict, auto_bool, auto_none which could auto doing extention and sharing without you to maully control the memory. Similar to Python built-in data type.

## Todo in this repo
* add a function called "to_string" to all of my types. int, float, bool, none, list, dict class, it needs to have a 'to_string' and 'from_string' functions, so that when I call 'to_string', it will print out a json_compatible string, when I call 'from_string', it will convert json string to memory object

* Use those types to make `json_loads(text)` and `json_dumps(dict)` functions
