#include "../y_python.h"
#include "../y_python_linux.h"

void process(Type_Ypython_String *text_code) {
    //split into lines and execute
    ypython_print(text_code->value);
    ypython_print("");

    Type_Ypython_Dict *variable_dict = Ypython_Dict();

    Type_Ypython_List *lines_list = ypython_string_type_function_split(text_code, Ypython_String("\n"));
    lines_list->function_start_iteration(lines_list);
    while (lines_list->iteration_not_done) {
        Type_Ypython_General *temp = lines_list->function_get_next_one(lines_list);
        if (!temp->is_none) {
            char *one_line = temp->string_->value;

            Type_Ypython_String *a_line = Ypython_String(one_line);
            if (a_line->function_is_substring(a_line, Ypython_String(" = "))) {
                Type_Ypython_List *part_list = ypython_string_type_function_split(a_line, Ypython_String(" = "));
                Type_Ypython_General *variable_name = part_list->function_get(part_list, 0);
                Type_Ypython_General *variable_value = part_list->function_get(part_list, 1);
                
                //ypython_print(variable_name->string_->value);
                //ypython_print(variable_value->string_->value);
                Type_Ypython_String *pure_value = variable_value->string_->function_substring(variable_value->string_, 1, variable_value->string_->length-1);
                variable_value->string_ = pure_value;

                variable_dict->function_set(variable_dict, variable_name->string_, variable_value);
            } else if ((a_line->function_is_substring(a_line, Ypython_String("print("))) && (a_line->function_is_substring(a_line, Ypython_String(")")))) {
                Type_Ypython_List *part_list = ypython_string_type_function_split(a_line, Ypython_String("print("));
                Type_Ypython_String *temp_string = part_list->function_get(part_list, 1)->string_;
                part_list = ypython_string_type_function_split(temp_string, Ypython_String(")"));
                temp_string = part_list->function_get(part_list, 0)->string_;
                Type_Ypython_String *variable_name = temp_string;

                Type_Ypython_General *variable_value = variable_dict->function_get(variable_dict, variable_name);
                ypython_print(variable_value->string_->value);
            }
        }
    }
}

int main(int argument_number, char **argument_list) {
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
            ypython_print(file_path->value);
            ypython_print("Make sure your python file exists!");
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
            process(file_content);
        }
        _ypython_file_close(a_file);
    }

    return 0;
}
