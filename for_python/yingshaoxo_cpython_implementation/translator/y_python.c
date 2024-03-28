#include "../y_python.h"
#include "../y_python_linux.h"

int main() {
    //clear_screen();

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
}
