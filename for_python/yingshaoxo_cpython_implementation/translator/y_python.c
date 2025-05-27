#include "../y_python.h"
#include "../y_python_linux.h"

// Global dictionary
Type_Ypython_Dict *global_variable_dict = NULL;

typedef struct Type_Ypython_Element_Instance Type_Ypython_Element_Instance;
struct Type_Ypython_Element_Instance {
    // none, string, bool, int, float, list, dict, function(a_string_of_code_block), class, class_instance(propertys:dict{...variable_dict, ...functions.dict})
    Type_Ypython_String *_type;
    Type_Ypython_String *_name; // variable name, function name, class name
    Type_Ypython_General *_value; // in c, it is Ypython_General()
};

Type_Ypython_Element_Instance *Ypython_Element_Instance() {
    Type_Ypython_Element_Instance *new_element_instance;
    new_element_instance = (Type_Ypython_Element_Instance *)malloc(sizeof(Type_Ypython_Element_Instance));

    new_element_instance->_type = Ypython_String("");
    new_element_instance->_name = Ypython_String("");
    new_element_instance->_value = NULL;
    
    return new_element_instance;
}

bool is_digital(Type_Ypython_String *a_string) {
    Type_Ypython_String *new_string = a_string->function_strip(a_string, Ypython_String("   \n1234567890."));
    if (new_string->function_is_equal(new_string, Ypython_String(""))) {
        return true;
    } else {
        return false;
    }
}

void process(Type_Ypython_String *text_code, Type_Ypython_Dict *variable_dict) {
    Type_Ypython_List *lines_list = ypython_string_type_function_split(text_code, Ypython_String("\n"));
    int line_index = 0;
    while (line_index < lines_list->length) {
        Type_Ypython_General *temp = lines_list->function_get(lines_list, line_index);
        if (!temp->is_none) {
            Type_Ypython_String *original_line = Ypython_String(temp->string_->value);
            Type_Ypython_String *a_line = Ypython_String(original_line->value);
            a_line = a_line->function_strip(a_line, Ypython_String("    \n"));

            if (a_line->function_startswith(a_line, Ypython_String("#"))) {
                // do nothing
            } else if (a_line->function_is_substring(a_line, Ypython_String(" = "))) {
                Type_Ypython_List *part_list = ypython_string_type_function_split(a_line, Ypython_String(" = "));
                Type_Ypython_General *variable_name = part_list->function_get(part_list, 0);
                Type_Ypython_General *variable_value = part_list->function_get(part_list, 1);
                
                Type_Ypython_String *pure_value = variable_value->string_->function_substring(variable_value->string_, 1, variable_value->string_->length-1);
                variable_value->string_ = pure_value;
                
                // Set the value in the dictionary
                Type_Ypython_Element_Instance *an_element = Ypython_Element_Instance();
                an_element->_type = Ypython_String("string");
                an_element->_name = Ypython_String(variable_name->string_->value);
                an_element->_value = variable_value;

                Type_Ypython_General *a_general_variable_that_can_hold_anything = Ypython_General();
                a_general_variable_that_can_hold_anything->anything_ = an_element;
                
                variable_dict->function_set(variable_dict, variable_name->string_, a_general_variable_that_can_hold_anything);
            } else if ((a_line->function_is_substring(a_line, Ypython_String("print("))) && (a_line->function_is_substring(a_line, Ypython_String(")")))) {
                Type_Ypython_List *part_list = ypython_string_type_function_split(a_line, Ypython_String("print("));
                Type_Ypython_String *temp_string = part_list->function_get(part_list, 1)->string_;
                part_list = ypython_string_type_function_split(temp_string, Ypython_String(")"));
                temp_string = part_list->function_get(part_list, 0)->string_;
                
                // Create a new string for the variable name
                Type_Ypython_String *variable_name = Ypython_String(temp_string->value);
                
                // Get the value from dictionary
                Type_Ypython_General *an_general_value = variable_dict->function_get(variable_dict, variable_name);
                
                if (an_general_value != NULL && !an_general_value->is_none && an_general_value->anything_ != NULL) {
                    Type_Ypython_Element_Instance *an_element = (Type_Ypython_Element_Instance*)(an_general_value->anything_);
                    ypython_print(an_element->_value->string_);
                } else {
                    //ypython_print(variable_name);
                }
            } else if (a_line->function_is_substring(a_line, Ypython_String("def "))) {
                // Handle function definition
                Type_Ypython_List *part_list = ypython_string_type_function_split(a_line, Ypython_String("def "));
                Type_Ypython_String *temp_string = part_list->function_get(part_list, 1)->string_;
                part_list = ypython_string_type_function_split(temp_string, Ypython_String("("));
                Type_Ypython_String *function_name = Ypython_String(part_list->function_get(part_list, 0)->string_->value);
                
                // Collect function body
                Type_Ypython_String *function_body = Ypython_String("");
                long long a_index = 0;
                while (a_index < lines_list->length) {
                    temp = lines_list->function_get(lines_list, a_index);
                    if (!temp->is_none) {
                        a_line = Ypython_String(temp->string_->value);
                        
                        if (a_line->function_is_substring(a_line, Ypython_String("    "))) {
                            Type_Ypython_String *clean_line = a_line->function_substring(a_line, 4, a_line->length);
                            function_body = function_body->function_add(function_body, clean_line);
                            function_body = function_body->function_add(function_body, Ypython_String("\n"));
                        } else {
                            break;
                        }
                    }
                    a_index += 1;
                }
                
                // Store function definition
                Type_Ypython_General *function_body_general = Ypython_General();
                function_body_general -> string_ = function_body;

                Type_Ypython_Element_Instance *an_element = Ypython_Element_Instance();
                an_element->_type = Ypython_String("function");
                an_element->_name = Ypython_String(function_name->value);
                an_element->_value = function_body_general;

                variable_dict->function_set(variable_dict, function_name, ypython_create_a_general_variable(an_element));
            } else if (a_line->function_endswith(a_line, Ypython_String(")"))) {
                // Handle function call
                Type_Ypython_List *part_list = ypython_string_type_function_split(a_line, Ypython_String("("));
                Type_Ypython_String *function_name = Ypython_String(part_list->function_get(part_list, 0)->string_->value);
                
                Type_Ypython_General *an_general_value = variable_dict->function_get(variable_dict, function_name);
                if ((an_general_value != NULL) && (!an_general_value->is_none)) {
                    Type_Ypython_General *function_body_general = (((Type_Ypython_Element_Instance*)(an_general_value->anything_))->_value);
                    if ((function_body_general != NULL) && (!function_body_general->is_none)) {
                        //ypython_print(function_body_general->string_);
                        Type_Ypython_Dict *new_dict = Ypython_Dict();
                        new_dict = _Ypython_dict_inheritance(variable_dict, new_dict);
                        process(function_body_general->string_, new_dict);
                    }
                }
            }

            line_index = line_index + 1;
        }
    }
}

int main(int argument_number, char **argument_list) {
    // Initialize global dictionaries
    global_variable_dict = Ypython_Dict();
    Type_Ypython_List *built_in_functions = Ypython_List();
    built_in_functions->function_append(built_in_functions, ypython_create_a_general_variable(Ypython_String("type")));
    global_variable_dict->function_set(global_variable_dict, Ypython_String("__built_in_s__"), ypython_create_a_general_variable(built_in_functions));

    if (argument_number <= 1) {
        // work as an realtime intepreter console
        Type_Ypython_String *line = Ypython_String("");
        _ypython_print_formated_string(">> ");

        while (true) {
            char character;
            //_ypython_scan_formated_string("%c", &character);
            character = getc(stdin);

            if (character != '\n') {
                //printf("%c", character);
                char *character_string = malloc(sizeof(char) * 2);
                character_string[0] = character;
                character_string[1] = '\0';

                Type_Ypython_String *new_character = Ypython_String(character_string);
                line = line->function_add(line, new_character);
            } else {
                char *one_line_input = line -> value;

                char *one_line_command = malloc(sizeof(char) * 100);
                _ypython_string_format(one_line_command, "python3 -c 'print(%s)'", one_line_input);
                char *result = ypython_run_command(one_line_command);
                _ypython_print_formated_string(">> %s", result);

                line = Ypython_String("");
            }
        }
    } else {
        // parse a python file, and execute functions
        Type_Ypython_String *file_path = Ypython_String(argument_list[1]);

        if ((!ypython_disk_exists(file_path->value)) || (ypython_disk_is_folder(file_path->value))) {
            ypython_print(file_path);
            ypython_print(Ypython_String("Make sure your python file exists!"));
            exit(1);
        }

        FILE *a_file = _ypython_file_open(file_path->value, "r");
        if (a_file != NULL) {
            Type_Ypython_String *file_content = Ypython_String("");
            while (true) {
                if (_ypython_return_end_of_file_indicator_for_a_STREAM(a_file) == 1) {
                    break;
                };

                char character = _ypython_file_get_character(a_file);
                if ((character < 0) || (character >= 255)) {
                    // ignore non_ascii character
                    continue;
                }
                
                char *character_string = malloc(sizeof(char) * 2);
                character_string[0] = character;
                character_string[1] = '\0';
                Type_Ypython_String *new_character = Ypython_String(character_string);
                file_content = file_content->function_add(file_content, new_character);
            }

            process(file_content, global_variable_dict);
        }
        _ypython_file_close(a_file);
    }

    return 0;
}
