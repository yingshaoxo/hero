#include "../y_python.h"
#include "../y_python_linux.h"


int test_number = 0;


void print_seperator(const char * test_name) {
    test_number += 1;
    ypython_print("\n---------------\n");
    if (_ypython_string_is_string_equal(test_name, "")) {
        _ypython_print_formated_string("Test %d", test_number);
    } else {
        _ypython_print_formated_string("Test %d: %s", test_number, test_name);
    }
    ypython_print("\n\n↓\n");
}


int main()
{
    print_seperator("");

    const char *result = ypython_run_command("echo 'hi'");
    const char *new_result = ypython_string_strip((char *)result);
    if (_ypython_string_compare(new_result, "hi") != 0)
    {
        _ypython_print_formated_string("'hi' should equal to '%s'\n", new_result);
        exit(0);
    }

    ///////////////////////////////////////////////////////
    
    print_seperator("");

    ypython_run("uname -v");

    //////////////////////////////////////////////////////

    print_seperator("");

    _ypython_print_formated_string("Hi, '%s'.\n", "yingshaoxo");

    //////////////////////////////////////////////////////

    print_seperator("");

    Type_Ypython_Float *a_float = Ypython_Float(3.2);
    _ypython_print_formated_string("%.3Lf\n", a_float->value);

    Type_Ypython_Float *another_float = Ypython_Float(1.8);
    Type_Ypython_Float *sum = a_float-> function_add(a_float, another_float);
    _ypython_print_formated_string("%.3Lf\n", sum->value);

    long double sum_float = 5.0;
    if (_ypython_get_float_absolute_value(sum->value - sum_float) > 0.0001) {
        _ypython_print_formated_string("%.8Lf should equal to %.8Lf\n", sum->value, sum_float);
    }

    //////////////////////////////////////////////////////

    print_seperator("");

    Type_Ypython_Int *a_int = Ypython_Int(3);
    _ypython_print_formated_string("%lld\n", a_int->value);

    Type_Ypython_Int *another_int = Ypython_Int(2);
    Type_Ypython_Int *sum_int = a_int-> function_add(a_int, another_int);
    _ypython_print_formated_string("%lld\n", sum_int->value);

    long long sum_int2 = 5;
    if (sum_int2 != sum_int->value) {
        _ypython_print_formated_string("%lld should equal to %lld\n", sum_int->value, sum_int2);
    }

    //////////////////////////////////////////////////////

    print_seperator("");

    Type_Ypython_String *a_string = Ypython_String("yingshaoxo");
    ypython_print(a_string->value);

    Type_Ypython_String *another_string = Ypython_String(" is super nice.");
    Type_Ypython_String *final_string = a_string->function_add(a_string, another_string);

    Type_Ypython_String *target_string = Ypython_String("yingshaoxo is super nice.");
    if (!(target_string->function_is_equal(target_string, final_string))) {
        //ypython_print(target_string->value);
        //ypython_print(final_string->value);
        _ypython_print_formated_string("'%s' should equal to '%s'\n", final_string->value, target_string->value);
    }

    //////////////////////////////////////////////////////

    print_seperator("");

    if (!_ypython_string_is_sub_string("abc", "ab")) {
        ypython_print("'ab' should be a substring of 'abc'");
    };

    if (_ypython_string_is_sub_string("abc", "ac")) {
        ypython_print("'ab' should not be a substring of 'abc'");
    };

    if (_ypython_string_count_sub_string("abc abc acbc", "abc") != 2) {
        ypython_print("'abc' should appeared 2 times in 'abc abc acbc'");
    };

    //////////////////////////////////////////////////////

    print_seperator("");

    a_string = Ypython_String("nice");
    if (!(a_string->function_is_equal(Ypython_String(a_string->type), Ypython_String("string")))) {
        ypython_print("type should be 'string'");
    }

    //////////////////////////////////////////////////////

    print_seperator("General Type");

    Type_Ypython_String *a_string_2 = Ypython_String("nice");

    Type_Ypython_General *general_variable = Ypython_General();
    general_variable->string_ = a_string_2;
    ypython_print(general_variable->string_->value);

    if (general_variable->string_->function_is_equal(general_variable->string_, Ypython_String("nice")) == false) {
        ypython_print("Error in general type");
    }
    
    //////////////////////////////////////////////////////
    
    print_seperator("List Test");

    Type_Ypython_List *a_list = Ypython_List();

    Type_Ypython_General *general_variable_2 = Ypython_General();
    Type_Ypython_String *a_string_3 = Ypython_String("element in list");
    general_variable_2->string_ = a_string_3;

    a_list->function_append(a_list, general_variable_2);
    ypython_print(a_list->function_get(a_list, 0)->string_->value);

    _ypython_print_formated_string("%lld\n", a_list->length);
    a_list->function_delete(a_list, 0);
    _ypython_print_formated_string("%lld\n", a_list->length);
    a_list->function_insert(a_list, 0, general_variable_2);
    _ypython_print_formated_string("%lld\n", a_list->length);
    ypython_print(a_list->function_get(a_list, 0)->string_->value);
    /*

    Type_Ypython_Int *index = a_list->function_index(a_list, general_variable_2);
    _ypython_print_formated_string("%lld\n", index->value);
    */

    //////////////////////////////////////////////////////
    
    print_seperator("General type compare Test");

    Type_Ypython_General *general_variable_3 = Ypython_General();
    Type_Ypython_String *a_string_4 = Ypython_String("ok");
    general_variable_3->string_ = a_string_4;

    Type_Ypython_General *general_variable_4 = Ypython_General();
    Type_Ypython_String *a_string_5 = Ypython_String("ok");
    general_variable_4->string_ = a_string_5;

    if (general_variable_4->function_is_equal(general_variable_3, general_variable_4) == false) {
        ypython_print("Error in general type compare");
    }

    Type_Ypython_String *a_string_6 = Ypython_String("jilkjalsdhfkj in list");
    general_variable_4->string_ = a_string_6;

    if (general_variable_4->function_is_equal(general_variable_3, general_variable_4) == true) {
        ypython_print("Error in general type compare");
    }

    //////////////////////////////////////////////////////
    
    print_seperator("Dict Test");

    Type_Ypython_General *general_variable_7 = Ypython_General();
    Type_Ypython_String *a_string_7 = Ypython_String("yingshaoxo");
    general_variable_7->string_ = a_string_7;

    Type_Ypython_Dict *a_dict = Ypython_Dict();
    Type_Ypython_String *the_key_7 = Ypython_String("god");
    a_dict->function_set(a_dict, the_key_7, general_variable_7);

    Type_Ypython_General *general_variable_8 = a_dict->function_get(a_dict, the_key_7);
    ypython_print(general_variable_8->string_->value);

    Type_Ypython_General *general_variable_9 = Ypython_General();
    Type_Ypython_String *a_string_9 = Ypython_String("yingshaoxo2");
    general_variable_9->string_ = a_string_9;

    Type_Ypython_String *the_key_8 = Ypython_String("404 censorship");
    a_dict->function_set(a_dict, the_key_8, general_variable_9);

    a_dict->keys->function_start_iteration(a_dict->keys);
    ypython_print("dict loop start----\n");
    while (a_dict->keys->iteration_not_done) {
        Type_Ypython_General *temp = a_dict->keys->function_get_next_one(a_dict->keys);
        if (!temp->is_none) {
            ypython_print(temp->string_->value);
            ypython_print(a_dict->function_get(a_dict, temp->string_)->string_->value);
        }
    }
    ypython_print("dict loop end----\n");

    ypython_print("for sublist: ");

    a_list = a_dict->keys->function_sublist(a_dict->keys, 0, 1);
    a_list->function_start_iteration(a_list);
    while (a_list->iteration_not_done) {
        Type_Ypython_General *temp = a_list->function_get_next_one(a_list);
        if (!temp->is_none) {
            ypython_print(temp->string_->value);
        }
    }


    //////////////////////////////////////////////////////
    
    print_seperator("startswith Test");

    Type_Ypython_String *a_string_10 = Ypython_String("yingshaoxo2");
    Type_Ypython_String *a_string_11 = Ypython_String("yingsh");
    if (!a_string_10->function_startswith(a_string_10, a_string_11)) {
        ypython_print("Error: startswith not work");
    }

    //////////////////////////////////////////////////////
    
    print_seperator("endswith Test");

    a_string_10 = Ypython_String("yingshaoxo");
    a_string_11 = Ypython_String("xo");
    if (!a_string_10->function_endswith(a_string_10, a_string_11)) {
        ypython_print("Error: endswith not work");
    }


    //////////////////////////////////////////////////////
    
    print_seperator("strip Test");

    Type_Ypython_String *a_string_12 = Ypython_String("    yingshaoxo22  ");
    Type_Ypython_String *a_string_13 = Ypython_String(" 2y");
    ypython_print(a_string_12->function_strip(a_string_12, a_string_13)->value);

    //////////////////////////////////////////////////////
    
    print_seperator("is_sub_string Test");

    Type_Ypython_String *a_string_14 = Ypython_String("yingshaoxo2");
    Type_Ypython_String *a_string_15 = Ypython_String("shao");
    if (a_string_14->function_is_substring(a_string_14, a_string_15)) {
        ypython_print("ok");
    }


    //////////////////////////////////////////////////////
    
    print_seperator("sub_string Test");

    Type_Ypython_String *a_string_16 = Ypython_String("yingshaoxo2");
    ypython_print(a_string_16->function_substring(a_string_16, 0, 11)->value);

    //////////////////////////////////////////////////////
    
    print_seperator("string split Test");

    Type_Ypython_String *a_string_17 = Ypython_String("yingshaoxo is god");

    Type_Ypython_List *a_list2 = ypython_string_type_function_split(a_string_17, Ypython_String(" "));
    a_list2->function_start_iteration(a_list2);
    while (a_list2->iteration_not_done) {
        Type_Ypython_General *temp = a_list2->function_get_next_one(a_list2);
        if (!temp->is_none) {
            ypython_print(temp->string_->value);
        }
    }

    ypython_raw_print(ypython_string_type_function_split(Ypython_String("yingshaoxo"), NULL));

    //////////////////////////////////////////////////////
    
    print_seperator("create general variable from a specific variable Test");

    Type_Ypython_String *a_string_18 = Ypython_String("yingshaoxo is best");

    Type_Ypython_General *a_general_variable_1 = ypython_create_a_general_variable(a_string_18);
    ypython_print(a_general_variable_1->string_->value);
}
