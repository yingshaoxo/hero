// #ifndef stdio.h
// #define stdio.h
#include <stdio.h>
// #endif
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

/*
Say hello to yingshaoxo.
*/
void hello_yingshaoxo()
{
    printf("Hello, world!\n");
}

/*
##################################################
Let's give those built-in c functions another name
##################################################
*/

// We could directly use '#define' to set a global replacement for function, but for the argurments hint, I choose not to do so.
// #define _ypython_resize_memory_block(buf,size) realloc(buf, size)

/*
Print formated string.
*/
/*
#define _ypython_print_formated_string(string_format, ...) printf(string_format, __VA_ARGS__);                \
*/
void _ypython_print_formated_string(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    vprintf(format, arg);
    va_end(arg);
    fflush(stdout);
}

/*
The sprintf() function is used to print formatted data to buffer.
*/
#define _ypython_string_format(result_string, string_format, ...) sprintf(result_string, string_format, __VA_ARGS__);

/*
`scanf` is a function that stands for Scan Formatted String. It is used to read data from stdin (standard input stream i.e. usually keyboard) and then writes the result into the given arguments.

It accepts character, string, and numeric data from the user using standard input.
scanf also uses format specifiers like printf.
*/
#define _ypython_scan_formated_string(string_format, ...) scanf(string_format, __VA_ARGS__);                \

/*
Get the absolute value of a floating point number.
*/

double _ypython_get_float_absolute_value(double __x) {
    return fabs(__x);
}

/*
The C library function `void *realloc(void *ptr, size_t size)` attempts to resize the memory block pointed to by `ptr` that was previously allocated with a call to `malloc` or `calloc`.
*/
void *_ypython_resize_memory_block_for_a_pointer(void *__ptr, size_t __size)
{
    return realloc(__ptr, __size);
}

/*
Get a newline-terminated string of finite length from STREAM.

This function is a possible cancellation point and therefore not
marked with __THROW.
*/
char *_ypython_get_a_newline_terminated_string_of_finite_length_from_a_STREAM(char *__restrict__ __s, int __n, FILE *__restrict__ __stream)
{
    return fgets(__s, __n, __stream);
}

/*
Return the EOF(end of file) indicator for STREAM.
*/
int _ypython_return_end_of_file_indicator_for_a_STREAM(FILE *__stream)
{
    return feof(__stream);
}

/*
The C library function `size_t strlen(const char *str)` computes the length of the string `str` up to, but not including the terminating null character.
*/
size_t _ypython_get_string_length(const char *__s)
{
    return strlen(__s);
}

/*
The popen function executes the shell command command as a subprocess.
However, instead of waiting for the command to complete, it creates a pipe to the subprocess and returns a stream that corresponds to that pipe.
*/
FILE *_ypython_execute_shell_command_as_a_subprocess_pipe_stream(const char *command, const char *mode)
{
    return popen(command, mode);
}

/*
The pclose() function closes a stream that was opened by popen(), waits for the command specified as an argument in popen() to terminate.
It returns the status of the process that was running the shell command.
*/
int _ypython_close_subprocess_pipe_stream(FILE *stream)
{
    return pclose(stream);
}

/*
The strcmp() compares two strings character by character. If the strings are equal, the function returns 0.
*/
int _ypython_string_compare(const char *str1, const char *str2)
{
    return strcmp(str1, str2);
}

/*
The C library function `char *strcat(char *dest, const char *src)` appends the string pointed to by src to the end of the string pointed to by dest.
In other words, second string will get added to the end of the first string.
*/
char *_ypython_string_adding(char *dest, const char *src) {
    return strcat(dest, src);
}

/*
The C library function `char *strstr(const char *haystack, const char *needle)` function finds the first occurrence of the substring needle in the string haystack. 
It will return null pointer if the sub_sequence is not present in haystack.
The terminating '\0' characters are not compared.
*/
const char *_ypython_find_the_first_sub_string_in_a_string(const char *a_string, const char *sub_string) {
    return strstr(a_string, sub_string);
}


/*
##################################################
Let's use those built-in c functions to do some useful things
##################################################
*/

bool _ypython_string_is_string_equal(const char* x, const char* y)
{
    int flag = 0;
 
    // Iterate a loop till the end
    // of both the strings
    while (*x != '\0' || *y != '\0') {
        if (*x == *y) {
            x++;
            y++;
        }
 
        // If two characters are not same
        // print the difference and exit
        else if ((*x == '\0' && *y != '\0')
                 || (*x != '\0' && *y == '\0')
                 || *x != *y) {
            flag = 1;
            break;
        }
    }
 
    // If two strings are exactly same
    if (flag == 0) {
        return true;
    } else {
        return false;
    }
}

bool _ypython_string_is_sub_string(const char *a_string, const char *sub_string) {
    if (_ypython_find_the_first_sub_string_in_a_string(a_string, sub_string) != NULL) {
        return true;
    } else {
        return false;
    }
}

long long _ypython_string_count_sub_string(const char *a_string, const char *sub_string) {
    long long counting = 0;
    const char *temprary = a_string;
    temprary = _ypython_find_the_first_sub_string_in_a_string(temprary, sub_string);
    while (temprary != NULL) {
        counting++;

        temprary++;
        temprary = _ypython_find_the_first_sub_string_in_a_string(temprary, sub_string);
    }
    return counting;
}

bool _ypython_is_general_space(char c)
{
    switch (c)
    {
    case ' ':
    case '\n':
    case '\t':
    case '\f':
    case '\r':
        return true;
        break;
    default:
        return false;
        break;
    }
}

char *_ypython_string_left_strip(char *s)
{
    while (_ypython_is_general_space(*s))
    {
        s++;
    }
    return s;
}

char *_ypython_string_right_strip(char *s)
{
    char *back = s + strlen(s) - 1;
    while (_ypython_is_general_space(*back))
    {
        --back;
    }
    *(back + 1) = '\0';
    return s;
}

/*
strip \s before and after a string.
*/
const char *ypython_string_strip(char *s)
{
    return (const char *)_ypython_string_right_strip(_ypython_string_left_strip(s));
}

char *_ypython_get_infinate_length_text_line(FILE *f)
{
    size_t size = 0;
    size_t len = 0;
    size_t last = 0;
    char *buf = NULL;

    do
    {
        size += BUFSIZ;                                              /* BUFSIZ is defined as "the optimal read size for this platform" */
        buf = (char *) _ypython_resize_memory_block_for_a_pointer(buf, size); /* realloc(NULL,n) is the same as malloc(n) */
        /* Actually do the read. Note that fgets puts a terminal '\0' on the
           end of the string, so we make sure we overwrite this */
        if (buf == NULL)
            return NULL;
        _ypython_get_a_newline_terminated_string_of_finite_length_from_a_STREAM(buf + last, BUFSIZ, f);
        len = _ypython_get_string_length(buf);
        last = len - 1;
    } while (!_ypython_return_end_of_file_indicator_for_a_STREAM(f) && buf[last] != '\n');

    return buf;
}

char *_ypython_get_infinate_length_text(FILE *f)
{
    size_t size = 0;
    size_t len = 0;
    size_t last = 0;
    char *buf = NULL;

    do
    {
        size += BUFSIZ;                                              /* BUFSIZ is defined as "the optimal read size for this platform" */
        buf = (char *)_ypython_resize_memory_block_for_a_pointer(buf, size); /* realloc(NULL,n) is the same as malloc(n) */
        /* Actually do the read. Note that fgets puts a terminal '\0' on the
           end of the string, so we make sure we overwrite this */
        if (buf == NULL)
            return NULL;
        _ypython_get_a_newline_terminated_string_of_finite_length_from_a_STREAM(buf + last, BUFSIZ, f);
        len = _ypython_get_string_length(buf);
        last = len - 1;

        if (buf[last] == '\n')
        {
            last = len;
        }
    } while (!_ypython_return_end_of_file_indicator_for_a_STREAM(f));

    return buf;
}

/*
Run a bash command and return the result as a string.
*/
/*
const char *ypython_run_command(const char *bash_command_line)
{
    FILE *FileOpen;
    FileOpen = _ypython_execute_shell_command_as_a_subprocess_pipe_stream(bash_command_line, "r");

    char *all_lines = _ypython_get_infinate_length_text(FileOpen);
    _ypython_close_subprocess_pipe_stream(FileOpen);

    return all_lines;
}
*/

/*
Run a bash command and wait for it to get finished, it won't return anything.
*/
/*
void ypython_run(const char *bash_command_line)
{
    FILE *FileOpen;
    FileOpen = _ypython_execute_shell_command_as_a_subprocess_pipe_stream(bash_command_line, "r");

    while (!_ypython_return_end_of_file_indicator_for_a_STREAM(FileOpen))
    {
        char *a_line = _ypython_get_infinate_length_text_line(FileOpen);
        printf("%s", a_line);
    }

    _ypython_close_subprocess_pipe_stream(FileOpen);
}
*/

/*
Python_like print function.
*/
void ypython_print(const char *text)
{
    printf("%s\n", text);
}


/*
##################################################
Let's create some built-in data types that similar to python data types
For example, str, double, int, bool, dict, list and so on
##################################################
*/

/*
Forward declarations
*/
typedef struct Type_Ypython_None Type_Ypython_None;
typedef struct Type_Ypython_String Type_Ypython_String;
typedef struct Type_Ypython_Bool Type_Ypython_Bool;
typedef struct Type_Ypython_Int Type_Ypython_Int;
typedef struct Type_Ypython_Float Type_Ypython_Float;
typedef struct Type_Ypython_General Type_Ypython_General;
typedef struct _Ypython_Linked_List_Node _Ypython_Linked_List_Node;
typedef struct Type_Ypython_List Type_Ypython_List;
typedef struct Type_Ypython_Dict Type_Ypython_Dict;

Type_Ypython_None *Ypython_None();
Type_Ypython_Bool *Ypython_Bool(bool value);
Type_Ypython_String *Ypython_String(char *value);
Type_Ypython_Int *Ypython_Int(long long value);
Type_Ypython_Float *Ypython_Float(long double value);
Type_Ypython_General *Ypython_General();
Type_Ypython_List *Ypython_List();
Type_Ypython_Dict *Ypython_Dict();


/*
None type
*/
typedef struct Type_Ypython_None Type_Ypython_None;
struct Type_Ypython_None {
    bool is_none;
    char *type;
    int value;
};

Type_Ypython_None *Ypython_None() {
    Type_Ypython_None *new_none_value;
    new_none_value = (Type_Ypython_None *)malloc(sizeof(Type_Ypython_None));

    new_none_value->is_none = true;
    new_none_value -> type = (char *)"none";

    new_none_value->value = 0;

    return new_none_value;
}

/*
String type
*/
typedef struct Type_Ypython_String Type_Ypython_String;
struct Type_Ypython_String {
    bool is_none;
    char *type;

    char *value;
    long long length;

    Type_Ypython_String* (*function_add)(Type_Ypython_String *self, Type_Ypython_String *another_string);
    bool (*function_is_equal)(Type_Ypython_String *self, Type_Ypython_String *another_string);
    //Type_Ypython_List *(*function_split)(Type_Ypython_String *self, Type_Ypython_String *seperator_string);
    //Type_Ypython_String *(*function_join)(Type_Ypython_String *self, Type_Ypython_List *string_list, Type_Ypython_String *seperator_string);
    Type_Ypython_String *(*function_strip)(Type_Ypython_String *self, Type_Ypython_String *characters);
    bool (*function_startswith)(Type_Ypython_String *self, Type_Ypython_String *a_string);
    long long (*function_length)(Type_Ypython_String *self);
    bool (*function_is_substring)(Type_Ypython_String *self, Type_Ypython_String *a_string);
    Type_Ypython_String *(*function_substring)(Type_Ypython_String *self, long long start_index, long long end_index);
};

Type_Ypython_String *Ypython_String(char *value);
Type_Ypython_String *Type_Ypython_String_add(Type_Ypython_String *self, Type_Ypython_String *another_string) {
    Type_Ypython_String *new_string_value = Ypython_String((char *)"");

    if (self->is_none || another_string->is_none) {
        new_string_value->value = (char *)"";
        new_string_value->length = strlen(new_string_value->value);
        new_string_value->is_none = true;
    } else {
        new_string_value->is_none = false;

        size_t total_length = _ypython_get_string_length(self->value) + _ypython_get_string_length(another_string->value);
        char *new_chars_value = (char *)malloc(total_length);
        snprintf(new_chars_value, total_length+1, "%s%s", self->value, another_string->value);

        new_string_value->value = new_chars_value;
        new_string_value->length = strlen(new_string_value->value);
    }

    return new_string_value;
}

bool Type_Ypython_String_is_equal(Type_Ypython_String *self, Type_Ypython_String *another_string) {
    if (self->is_none && another_string->is_none) {
        return true;
    }
    else if ((!(self->is_none)) && (another_string->is_none)) {
        return false;
    }
    else if ((self->is_none) && (!(another_string->is_none))) {
        return false;
    } else {
        if (_ypython_string_is_string_equal(self->value, another_string->value)) {
            return true;
        } else {
            return false;
        }   
    }
}

//Type_Ypython_List *Type_Ypython_String_split(Type_Ypython_String *self, Type_Ypython_String *seperator_string) {
    /*
    Type_Ypython_List *result_list = Ypython_List();

    char *token;
    char *str = self->value;
    char *sep = seperator_string->value;

    token = strtok(str, sep);
    while(token != NULL) {
        Type_Ypython_String *new_string = Ypython_String("");
        new_string->value = strdup(token);

        Type_Ypython_General *a_general_variable = Ypython_General();
        a_general_variable->string_ = new_string;

        result_list->function_append(result_list, a_general_variable);

        token = strtok(NULL, sep);
    }

    return result_list;
    */
    //return NULL;
//}

/*
Type_Ypython_String *Type_Ypython_String_join(Type_Ypython_String *self, Type_Ypython_List *string_list, Type_Ypython_String *seperator_string) {
    return NULL;
}
*/

Type_Ypython_String *Type_Ypython_String_substring(Type_Ypython_String *self, long long start_index, long long end_index) {
    Type_Ypython_String *default_string = Ypython_String("");
    default_string->is_none = true;

    if (start_index > end_index) {
        return default_string;
    }

    if (self->is_none || ((start_index < 0) || (end_index > self->length))) {
        return default_string;
    }

    end_index -= 1;

    char *str = self->value;
    long long result_len = end_index - start_index + 1;
    char *result = (char *)malloc((result_len + 1) * sizeof(char));
    strncpy(result, str + start_index, result_len);
    result[result_len] = '\0';

    return Ypython_String(result);
}

bool Type_Ypython_String_is_substring(Type_Ypython_String *self, Type_Ypython_String *a_string) {
    char *str = self->value;
    char *substring = a_string->value;
    return _ypython_find_the_first_sub_string_in_a_string(str, substring) != NULL;
}

Type_Ypython_String *Type_Ypython_String_strip(Type_Ypython_String *self, Type_Ypython_String *characters) {
    long long len = strlen(self->value);
    char *str = self->value;

    long long start = 0;
    long long end = len - 1;

    if (characters != NULL) {
        char *ch = characters->value;

        while (start < len && strchr(ch, str[start]) != NULL) {
            start++;
        }

        while (end >= start && strchr(ch, str[end]) != NULL) {
            end--;
        }
    }

    return self->function_substring(self, start, end+1);
}

bool Type_Ypython_String_startswith(Type_Ypython_String *self, Type_Ypython_String *a_string) {
    char *str = self->value;
    char *prefix = a_string->value;
    int str_len = strlen(str);
    int prefix_len = strlen(prefix);

    if (str_len < prefix_len) {
        return false;
    }

    for (int i = 0; i < prefix_len; i++) {
        if (str[i] != prefix[i]) {
            return false;
        }
    }

    return true;
}

Type_Ypython_String *Ypython_String(char *value) {
    Type_Ypython_String *new_string_value;
    new_string_value = (Type_Ypython_String *)malloc(sizeof(Type_Ypython_String));

    new_string_value->is_none = false;
    new_string_value->type = (char *)"string";

    new_string_value->value = value;
    new_string_value->length = strlen(value);

    new_string_value->function_add = &Type_Ypython_String_add;
    new_string_value->function_is_equal = &Type_Ypython_String_is_equal;
    //new_string_value->function_split = &Type_Ypython_String_split;
    //new_string_value->function_join = &Type_Ypython_String_join;
    new_string_value->function_substring = &Type_Ypython_String_substring;
    new_string_value->function_is_substring = &Type_Ypython_String_is_substring;
    new_string_value->function_strip = &Type_Ypython_String_strip;
    new_string_value->function_startswith = &Type_Ypython_String_startswith;

    return new_string_value;
}

/*
Bool type
*/
typedef struct Type_Ypython_Bool Type_Ypython_Bool;
struct Type_Ypython_Bool {
    bool is_none;
    char *type;
    bool value;
};

Type_Ypython_Bool *Ypython_Bool(bool value) {
    Type_Ypython_Bool *new_bool_value;
    new_bool_value = (Type_Ypython_Bool *)malloc(sizeof(Type_Ypython_Bool));

    new_bool_value->is_none = false;
    new_bool_value -> type = (char *)"bool";

    new_bool_value->value = value;

    return new_bool_value;
}

/*
Int type
*/
typedef struct Type_Ypython_Int Type_Ypython_Int;
struct Type_Ypython_Int {
    bool is_none;
    char *type;
    long long value;

    Type_Ypython_Int *(*function_add)(Type_Ypython_Int *self, Type_Ypython_Int *another_int);
    Type_Ypython_Int *(*function_multiply)(Type_Ypython_Int *self, Type_Ypython_Int *another_int);
};

Type_Ypython_Int *Ypython_Int(long long value);

Type_Ypython_Int *Type_Ypython_Int_add(Type_Ypython_Int *self, Type_Ypython_Int *another_int) {
    Type_Ypython_Int *new_int_value = Ypython_Int(0);

    if (self->is_none || another_int->is_none) {
        new_int_value->value = 0;
        new_int_value->is_none = true;
        return new_int_value;
    } else {
        new_int_value->is_none = false;
        new_int_value->value = self->value + another_int->value;
        return new_int_value;
    }
}

Type_Ypython_Int *Type_Ypython_Int_multiply(Type_Ypython_Int *self, Type_Ypython_Int *another_int) {
    Type_Ypython_Int *new_int_value = Ypython_Int(0);

    if (self->is_none || another_int->is_none) {
        new_int_value->value = 0;
        new_int_value->is_none = true;
        return new_int_value;
    } else {
        new_int_value->is_none = false;
        new_int_value->value = self->value * another_int->value;
        return new_int_value;
    }
}

Type_Ypython_Int *Ypython_Int(long long value) {
    Type_Ypython_Int *new_int_value;
    new_int_value = (Type_Ypython_Int *)malloc(sizeof(Type_Ypython_Int));

    new_int_value->is_none = false;
    new_int_value->type = (char *)"int";

    new_int_value->value = value;

    new_int_value->function_add = &Type_Ypython_Int_add;
    new_int_value->function_multiply = &Type_Ypython_Int_multiply;

    return new_int_value;
}

/*
Float type
*/
typedef struct Type_Ypython_Float Type_Ypython_Float;
struct Type_Ypython_Float {
    bool is_none;
    char *type;
    long double value;

    Type_Ypython_Float *(*function_add)(Type_Ypython_Float *self, Type_Ypython_Float *another_float);
    Type_Ypython_Float *(*function_multiply)(Type_Ypython_Float *self, Type_Ypython_Float *another_float);
};

Type_Ypython_Float *Ypython_Float(long double value);

Type_Ypython_Float *Type_Ypython_Float_add(Type_Ypython_Float *self, Type_Ypython_Float *another_float) {
    Type_Ypython_Float *new_float_value = Ypython_Float(0.0);

    if (self->is_none || another_float->is_none) {
        new_float_value->value = 0;
        new_float_value->is_none = true;
        return new_float_value;
    } else {
        new_float_value->is_none = false;
        new_float_value->value = self->value + another_float->value;
        return new_float_value;
    }
}

Type_Ypython_Float *Type_Ypython_Float_multiply(Type_Ypython_Float *self, Type_Ypython_Float *another_float) {
    Type_Ypython_Float *new_float_value = Ypython_Float(0.0);

    if (self->is_none || another_float->is_none) {
        new_float_value->value = 0;
        new_float_value->is_none = true;
        return new_float_value;
    } else {
        new_float_value->is_none = false;
        new_float_value->value = self->value * another_float->value;
        return new_float_value;
    }
}

Type_Ypython_Float *Ypython_Float(long double value) {
    Type_Ypython_Float *new_float_value;
    new_float_value = (Type_Ypython_Float *)malloc(sizeof(Type_Ypython_Float));

    new_float_value->is_none = false;
    new_float_value->type = (char *)"float";

    new_float_value->value = value;

    new_float_value->function_add = &Type_Ypython_Float_add;
    new_float_value->function_multiply = &Type_Ypython_Float_multiply;

    return new_float_value;
}

/*
Forward declaration for: List, Dict
*/
typedef struct Type_Ypython_List Type_Ypython_List;
typedef struct Type_Ypython_Dict Type_Ypython_Dict;

/*
General type
*/
typedef struct Type_Ypython_General Type_Ypython_General;
struct Type_Ypython_General {
    bool is_none;
    char *type;

    Type_Ypython_Bool *bool_;
    Type_Ypython_Float *float_;
    Type_Ypython_Int *int_;
    Type_Ypython_String *string_;
    Type_Ypython_List *list_;
    Type_Ypython_Dict *dict_;

    bool (*function_is_equal)(Type_Ypython_General *element_1, Type_Ypython_General *element_2);
};

bool Type_Ypython_General_is_equal(Type_Ypython_General *element_1, Type_Ypython_General *element_2) {
    if (element_1->is_none && element_2->is_none) {
        return true;
    }
    else if ((!(element_1->is_none)) && (element_2->is_none)) {
        return false;
    }
    else if ((element_1->is_none) && (!(element_2->is_none))) {
        return false;
    } else {
        if ((element_1->bool_ != NULL) && (element_2->bool_ != NULL)) {
            if (element_1->bool_->is_none && element_2->bool_->is_none) {
                return true;
            } else if ((element_1->bool_->is_none) && (!element_2->bool_->is_none)) {
                return false;
            } else if ((!element_1->bool_->is_none) && (element_2->bool_->is_none)) {
                return false;
            } else if ((!element_1->bool_->is_none) && (!element_2->bool_->is_none)) {
                if (element_1->bool_->value == element_2->bool_->value) {
                    return true;
                }
            }
        }

        if ((element_1->float_ != NULL) && (element_2->float_ != NULL)) {
            if (element_1->float_->is_none && element_2->float_->is_none) {
                return true;
            } else if ((element_1->float_->is_none) && (!element_2->float_->is_none)) {
                return false;
            } else if ((!element_1->float_->is_none) && (element_2->float_->is_none)) {
                return false;
            } else if ((!element_1->float_->is_none) && (!element_2->float_->is_none)) {
                if (element_1->float_->value == element_2->float_->value) {
                    return true;
                }
            }
        }

        if ((element_1->int_ != NULL) && (element_2->int_ != NULL)) {
            if (element_1->int_->is_none && element_2->int_->is_none) {
                return true;
            } else if ((element_1->int_->is_none) && (!element_2->int_->is_none)) {
                return false;
            } else if ((!element_1->int_->is_none) && (element_2->int_->is_none)) {
                return false;
            } else if ((!element_1->int_->is_none) && (!element_2->int_->is_none)) {
                if (element_1->int_->value == element_2->int_->value) {
                    return true;
                }
            }
        }

        if ((element_1->string_ != NULL) && (element_2->string_ != NULL)) {
            if (element_1->string_->is_none && element_2->string_->is_none) {
                return true;
            } else if ((element_1->string_->is_none) && (!element_2->string_->is_none)) {
                return false;
            } else if ((!element_1->string_->is_none) && (element_2->string_->is_none)) {
                return false;
            } else if ((!element_1->string_->is_none) && (!element_2->string_->is_none)) {
                if (element_1->string_->function_is_equal(element_1->string_, element_2->string_)) {
                    return true;
                }
            }
        }
    }
    return false;
}

Type_Ypython_General *Ypython_General() {
    Type_Ypython_General *new_value;
    new_value = (Type_Ypython_General *)malloc(sizeof(Type_Ypython_General));

    new_value->is_none = false;
    new_value -> type = (char *)"general";

    new_value->bool_ = NULL;
    new_value->float_ = NULL;
    new_value->int_ = NULL;
    new_value->string_ = NULL;
    new_value->list_ = NULL;
    new_value->dict_ = NULL;

    new_value->function_is_equal = &Type_Ypython_General_is_equal;

    return new_value;
}

/*
List type

A good list data type has to have:
1. infinity list size increasing in real time
2. automatically garbage collection
*/
typedef struct _Ypython_Linked_List_Node _Ypython_Linked_List_Node;
struct _Ypython_Linked_List_Node {
    Type_Ypython_General *value;
    _Ypython_Linked_List_Node *next;
};

typedef struct Type_Ypython_List Type_Ypython_List;
struct Type_Ypython_List {
    bool is_none;
    char *type;

    _Ypython_Linked_List_Node *head;
    _Ypython_Linked_List_Node *tail;
    long long length;

    bool iteration_not_done;
    _Ypython_Linked_List_Node *_current_iterate_node;

    void (*function_append)(Type_Ypython_List *self, Type_Ypython_General *an_element);
    Type_Ypython_Int *(*function_index)(Type_Ypython_List *self, Type_Ypython_General *an_element);
    void (*function_delete)(Type_Ypython_List *self, long long index);
    void (*function_insert)(Type_Ypython_List *self, long long index, Type_Ypython_General *an_element);
    void (*function_set)(Type_Ypython_List *self, long long index, Type_Ypython_General *an_element);
    Type_Ypython_General* (*function_get)(Type_Ypython_List *self, long long index);
    Type_Ypython_List* (*function_sublist)(Type_Ypython_List *self, long long start_index, long long end_index);
    void (*function_start_iteration)(Type_Ypython_List *self);
    Type_Ypython_General* (*function_get_next_one)(Type_Ypython_List *self);
};

// Function to create a new linked list node
_Ypython_Linked_List_Node *_Ypython_create_list_Node(Type_Ypython_General *value) {
    _Ypython_Linked_List_Node *newNode = (_Ypython_Linked_List_Node *)malloc(sizeof(_Ypython_Linked_List_Node));
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

Type_Ypython_List *Ypython_List();
void Type_Ypython_List_append(Type_Ypython_List *self, Type_Ypython_General *an_element) {
    if (self->is_none) {
        return;
    }

    _Ypython_Linked_List_Node *newNode = _Ypython_create_list_Node(an_element);

    if (self->head == NULL) {
        self->head = newNode;
        self->tail = newNode;
    } else {
        self->tail->next = newNode;
        self->tail = newNode;
    }

    self->length = self->length + 1;
    return;
}

Type_Ypython_Int *Type_Ypython_List_index(Type_Ypython_List *self, Type_Ypython_General *an_element) {
    Type_Ypython_Int *index = Ypython_Int(-1);

    if (self->is_none) {
        index->is_none = true;
        return index;
    } else {
        int i = 0;
        _Ypython_Linked_List_Node *current_node = self->head;

        if (current_node == NULL) {
            // 0 elements inside;
            index->is_none = true;
            return index;
        }

        while (current_node != NULL) {
            if (current_node->value->function_is_equal(current_node->value, an_element)) {
                index->value = i;
                return index;
            }
            current_node = current_node->next;
            i++;
        }
    }

    index->is_none = true;
    return index;
}

void Type_Ypython_List_delete(Type_Ypython_List *self, long long index) {
    if (self->is_none || ((index < 0) || (index >= self->length))) {
        return;
    }

    int i = 0;
    _Ypython_Linked_List_Node *current_node = self->head;
    _Ypython_Linked_List_Node *previous = NULL;

    if (current_node == NULL) {
        // 0 elements inside;
        return;
    }
  
    while (current_node != NULL) {
        if (i == index) {
            if (previous == NULL) {
                // Deleting the head node
                self->head = current_node->next;
            } else {
                previous->next = current_node->next;
            }
            self->length = self->length - 1;
            free(current_node);
            break;
        }
        previous = current_node;
        current_node = current_node->next;
        i++;
    }
}

void Type_Ypython_List_insert(Type_Ypython_List *self, long long index, Type_Ypython_General *an_element) {
    if (self->is_none || ((index < 0) || (index > self->length))) {
        return;
    }

    int i = 0;
    _Ypython_Linked_List_Node *current_node = self->head;
    _Ypython_Linked_List_Node *previous = NULL;

    if (current_node == NULL) {
        // 0 elements inside;
        _Ypython_Linked_List_Node *newNode = _Ypython_create_list_Node(an_element);
        self->head = newNode;
        self->length = self->length + 1;
        return;
    }
  
    while (current_node != NULL) {
        if (i == index) {
            _Ypython_Linked_List_Node *newNode = _Ypython_create_list_Node(an_element);
          
            if (previous == NULL) {
                // Inserting at the head
                newNode->next = self->head;
                self->head = newNode;
            } else {
                newNode->next = current_node;
                previous->next = newNode;
            }
            
            self->length = self->length + 1;
            break;
        }
        previous = current_node;
        current_node = current_node->next;
        i++;
    }
}

void Type_Ypython_List_set(Type_Ypython_List *self, long long index, Type_Ypython_General *an_element) {
    if (self->is_none || ((index < 0) || (index >= self->length))) {
        return;
    }

    int i = 0;
    _Ypython_Linked_List_Node *current_node = self->head;
    _Ypython_Linked_List_Node *previous = NULL;

    if (current_node == NULL) {
        // 0 elements inside;
        return;
    }
  
    while (current_node != NULL) {
        if (i == index) {
            _Ypython_Linked_List_Node *newNode = _Ypython_create_list_Node(an_element);
          
            if (previous == NULL) {
                // Inserting at the head
                newNode->next = self->head->next;
                free(self->head);
                self->head = newNode;
            } else {
                previous->next = newNode;
                newNode->next = current_node->next;
                free(current_node);
            }
            
            break;
        }
        previous = current_node;
        current_node = current_node->next;
        i++;
    }
}

Type_Ypython_General* Type_Ypython_List_get(Type_Ypython_List *self, long long index) {
    Type_Ypython_General *default_element = Ypython_General();
    default_element->is_none = true;

    if (self->is_none || ((index < 0) || (index >= self->length))) {
        return default_element;
    }

    int i = 0;
    _Ypython_Linked_List_Node *current_node = self->head;
    _Ypython_Linked_List_Node *previous = NULL;

    if (current_node == NULL) {
        // 0 elements inside;
        return default_element;
    }
  
    while (current_node != NULL) {
        if (i == index) {
            free(default_element);
            return current_node->value;
        }
        previous = current_node;
        current_node = current_node->next;
        i++;
    }

    return default_element;
}

Type_Ypython_List* Type_Ypython_List_sublist(Type_Ypython_List *self, long long start_index, long long end_index) {
    Type_Ypython_List *default_list = Ypython_List();
    default_list->is_none = true;

    if (start_index > end_index) {
        return default_list;
    }

    if (self->is_none || ((start_index < 0) || (end_index > self->length))) {
        return default_list;
    }

    int i = 0;
    _Ypython_Linked_List_Node *current_node = self->head;
    _Ypython_Linked_List_Node *previous = NULL;

    if (current_node == NULL) {
        // 0 elements inside;
        return default_list;
    }

    default_list->is_none = false;
  
    while (current_node != NULL) {
        if ((i >= start_index) && (i < end_index)) {
            default_list->function_append(default_list, current_node->value);
        } 
        if (i >= end_index) {
            break;
        }
        previous = current_node;
        current_node = current_node->next;
        i++;
    }

    return default_list;
}

void Type_Ypython_List_start_iteration(Type_Ypython_List *self) {
    if (self->is_none) {
        return;
    }

    self->_current_iterate_node = self->head;
    self->iteration_not_done = true;

    if (self->_current_iterate_node == NULL) {
        self->iteration_not_done = false;
    }
}

Type_Ypython_General* (Type_Ypython_List_get_next_one)(Type_Ypython_List *self) {
    Type_Ypython_General *default_element = Ypython_General();
    default_element->is_none = true;

    if (self->is_none) {
        self->iteration_not_done = false;
        return default_element;
    }

    if (self->_current_iterate_node == NULL) {
        self->iteration_not_done = false;
        return default_element;
    }

    default_element = self->_current_iterate_node->value;
    self->_current_iterate_node = self->_current_iterate_node->next;
    return default_element;
}

Type_Ypython_List *Ypython_List() {
    Type_Ypython_List *new_list_value;
    new_list_value = (Type_Ypython_List *)malloc(sizeof(Type_Ypython_List));

    new_list_value->is_none = false;
    new_list_value->type = (char *)"list";

    new_list_value->head = NULL;
    new_list_value->tail = NULL;
    new_list_value->length = 0;

    new_list_value->function_append = &Type_Ypython_List_append;
    new_list_value->function_index = &Type_Ypython_List_index;
    new_list_value->function_delete = &Type_Ypython_List_delete;
    new_list_value->function_insert = &Type_Ypython_List_insert;
    new_list_value->function_set = &Type_Ypython_List_set;
    new_list_value->function_get = &Type_Ypython_List_get;
    new_list_value->function_sublist = &Type_Ypython_List_sublist;
    new_list_value->function_start_iteration = &Type_Ypython_List_start_iteration;
    new_list_value->function_get_next_one = &Type_Ypython_List_get_next_one;

    return new_list_value;
}


/*
Dict type
//You can use 2 dimentional array, one to store the key, another to store the value, and two array uses same index and length.
//Key will always be Type_Ypython_String type inside Type_Ypython_General
*/
typedef struct Type_Ypython_Dict Type_Ypython_Dict;
struct Type_Ypython_Dict {
    bool is_none;
    char *type;

    Type_Ypython_List* keys;
    Type_Ypython_List* values;

    void (*function_set)(Type_Ypython_Dict *self, Type_Ypython_String *a_key, Type_Ypython_General *a_value);
    Type_Ypython_General *(*function_get)(Type_Ypython_Dict *self, Type_Ypython_String *a_key);
};

void Type_Ypython_Dict_set(Type_Ypython_Dict *self, Type_Ypython_String *a_key, Type_Ypython_General *a_value) {
    if (self->is_none) {
        return;
    } 

    if (self->keys == NULL || self->values == NULL) {
        return;
    }

    Type_Ypython_General *the_key = Ypython_General();
    the_key->string_ = a_key;

    Type_Ypython_Int *index = self->keys->function_index(self->keys, the_key);
    if (index->is_none) {
        // we don't have this key in this dict, add a new one
        self->keys->function_append(self->keys, the_key);
        self->values->function_append(self->values, a_value);
    } else {
        // we have this key in this dict, update old one
        self->values->function_set(self->values, index->value, a_value);
    }
}

Type_Ypython_General *Type_Ypython_Dict_get(Type_Ypython_Dict *self, Type_Ypython_String *a_key) {
    Type_Ypython_General *result = Ypython_General();

    if (self->is_none) {
        result->is_none = true;
        return result;
    } 

    if (self->keys == NULL || self->values == NULL) {
        result->is_none = true;
        return result;
    }

    Type_Ypython_General *the_key = Ypython_General();
    the_key->string_ = a_key;

    Type_Ypython_Int *index = self->keys->function_index(self->keys, the_key);
    if (index->is_none) {
        // we don't have this key in this dict, return none
        result->is_none = true;
        return result;
    } else {
        // we have this key in this dict, return the value
        return self->values->function_get(self->values, index->value);
    }
}

Type_Ypython_Dict *Ypython_Dict() {
    Type_Ypython_Dict *new_value;
    new_value = (Type_Ypython_Dict *)malloc(sizeof(Type_Ypython_Dict));

    new_value->is_none = false;
    new_value -> type = (char *)"dict";

    new_value->keys = Ypython_List();
    new_value->values = Ypython_List();

    new_value->function_set = &Type_Ypython_Dict_set;
    new_value->function_get = &Type_Ypython_Dict_get;

    return new_value;
}


/*
Old non_linked_list List type
//https://dev.to/bekhruzniyazov/creating-a-python-like-list-in-c-4ebg

A good list data type has to have:
1. infinity list size increasing in real time
2. automatically garbage collection
*/
/*
typedef struct Type_Ypython_List Type_Ypython_List;
struct Type_Ypython_List {
    bool is_none;
    char *type;

    Type_Ypython_General* *value;
    long long length;

    Type_Ypython_List *(*function_append)(Type_Ypython_List *self, Type_Ypython_General *an_element);
    Type_Ypython_Int *(*function_index)(Type_Ypython_List *self, Type_Ypython_General *an_element);
    Type_Ypython_List *(*function_delete)(Type_Ypython_List *self, long long index);
    Type_Ypython_List *(*function_insert)(Type_Ypython_List *self, long long index, Type_Ypython_General *an_element);
};

Type_Ypython_List *Ypython_List();
Type_Ypython_List *Type_Ypython_List_append(Type_Ypython_List *self, Type_Ypython_General *an_element) {
    Type_Ypython_List *new_list_value = Ypython_List();

    if (self->is_none) {
        new_list_value->is_none = true;
        return new_list_value;
    } else {
        new_list_value->length = self->length + 1;
        new_list_value->value = malloc(sizeof(Type_Ypython_General*) * new_list_value->length);

        // Copy the existing elements from the original list
        for (long long i = 0; i < self->length; i++) {
            new_list_value->value[i] = self->value[i];
        }

        // Append the new element to the end of the list
        new_list_value->value[new_list_value->length - 1] = an_element;
    }

    return new_list_value;
}

Type_Ypython_Int *Type_Ypython_List_index(Type_Ypython_List *self, Type_Ypython_General *an_element) {
    Type_Ypython_Int *index = Ypython_Int(-1);

    if (self->is_none) {
        index->is_none = true;
        return index;
    } else {
        for (long long i = 0; i < self->length; i++) {
            if (an_element->function_is_equal(an_element, self->value[i])) {
                index->value = i;
                return index;
            }
        }
    }

    index->is_none = true;
    return index;
}

Type_Ypython_List *Type_Ypython_List_delete(Type_Ypython_List *self, long long index) {
    Type_Ypython_List *new_list_value = Ypython_List();

    if (self->is_none) {
        new_list_value->is_none = true;
        return new_list_value;
    } else {
        new_list_value->length = self->length - 1;
        new_list_value->value = malloc(sizeof(Type_Ypython_General*) * new_list_value->length);

        // Copy the existing elements from the original list
        for (long long i = 0; i < self->length; i++) {
            if (i != index) {
                new_list_value->value[i] = self->value[i];
            }
        }
    }

    return new_list_value;
}

Type_Ypython_List *Type_Ypython_List_insert(Type_Ypython_List *self, long long index, Type_Ypython_General *an_element) {
    Type_Ypython_List *new_list_value = Ypython_List();

    if (self->is_none) {
        new_list_value->is_none = true;
        return new_list_value;
    } else {
        new_list_value->length = self->length + 1;
        new_list_value->value = malloc(sizeof(Type_Ypython_General*) * new_list_value->length);

        // Copy the existing elements from the original list
        for (long long i = 0; i < new_list_value->length; i++) {
            if (i < index) {
                new_list_value->value[i] = self->value[i];
            } else if (i == index) {
                new_list_value->value[i] = an_element;
            } else {
                new_list_value->value[i] = self->value[i-1];
            }
        }
    }

    return new_list_value;
}

Type_Ypython_List *Ypython_List() {
    Type_Ypython_List *new_list_value;
    new_list_value = (Type_Ypython_List *)malloc(sizeof(Type_Ypython_List));

    new_list_value->is_none = false;
    new_list_value->type = (char *)"list";

    new_list_value->length = 0;
    new_list_value->value = malloc(sizeof(Type_Ypython_General*) * new_list_value->length);

    new_list_value->function_append = &Type_Ypython_List_append;
    new_list_value->function_index = &Type_Ypython_List_index;
    new_list_value->function_delete = &Type_Ypython_List_delete;
    new_list_value->function_insert = &Type_Ypython_List_insert;

    return new_list_value;
}
*/

