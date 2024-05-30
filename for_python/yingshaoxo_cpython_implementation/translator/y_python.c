#include "../y_python.h"
#include "../y_python_linux.h"

void process(Type_Ypython_String *text_code) {
    //split into lines and execute
    ypython_print(text_code->value);
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
