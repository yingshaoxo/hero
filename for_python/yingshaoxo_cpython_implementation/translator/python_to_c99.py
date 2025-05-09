
def read_second_command_line_second_argument_file(file_path=None):
    if file_path == None:
        import sys
        if len(sys.argv) >= 2:
            file_path = sys.argv[1]

    txt = ""
    if file_path != None:
        with open(file_path, "r", encoding="utf-8") as f:
            txt = f.read()

    return txt

code_text = read_second_command_line_second_argument_file()
if code_text == "":
    # no code, read default parser
    code_text = read_second_command_line_second_argument_file("./y_python.py")



def get_indent_number(code):
    new_code = code.lstrip()
    return len(code) - len(new_code)

def get_indent(code):
    length = get_indent_number(code)
    return code[:length]

def set_indent(code, level=0):
    lines = code.split("\n")
    result_code = ""
    for line in lines:
        pure_line = line.strip()
        if pure_line != "":
            result_code += level*"    " + pure_line + "\n"
        else:
            result_code += "\n"
    return result_code

def get_text_until_closed_symbol(lines, start_symbol, end_symbol):
    result_text = ""

    start_symbol_counting = 0
    end_symbol_counting = 0

    code_text = "\n".join(lines)
    start_the_process = False
    for char in code_text:
        if start_symbol == char:
            start_symbol_counting += 1
            start_the_process = True
        elif end_symbol == char:
            end_symbol_counting += 1

        if start_the_process == True:
            result_text += char

            if start_symbol_counting == end_symbol_counting:
                break

    return result_text

class Python_Element_Instance():
    def __init__(self):
        # none, string, bool, int, float, list, dict, function(a_string_of_code_block), class, class_instance(propertys:dict{...variable_dict, ...functions.dict})
        self.type = "None"
        self.name = None # variable name, function name, class name
        self.general_value = None # in c, it is Ypython_General()

        self.is_in_global_level = True
        self.translated_code = ""
        self.translated_code_for_definition = ""
        self.translated_code_for_real_part = ""

def process_code(variable_dict, code_text, level=0):
    # level 0 means in global level
    default_element = Python_Element_Instance()
    element_list = []
    code_in_body = ""

    original_code_text = code_text
    code_text = code_text.strip()

    if code_text.startswith("{") and code_text.endswith("}"):
        # it is a dict
        default_element.type = "dict"
        default_element.general_value = code_text
        default_element.translated_code_for_definition = """Type_Ypython_Dict *{key} = NULL;"""
        default_element.translated_code_for_real_part = "{key} = Ypython_Dict();"
        default_element.translated_code = """
Type_Ypython_Dict *{key} = NULL;
{key} = Ypython_Dict();
"""
        return default_element
    else:
        # handle code text line by line for normal case
        lines = original_code_text.split("\n")
        line_index = 0
        while line_index < len(lines):
            original_line = lines[line_index]
            line = original_line.strip()
            # you have to implement the logic here, it is similar to what cython_to_c did, but the base python library is replaced by 'y_python.h' than 'Python.h'
            # 'y_python.h' is better because you can't easily do the static compile with 'Python.h', which makes other people hard to use your compiled c program
            if line.startswith("#"):
                code_in_body += get_indent(original_line) + "// " + line.lstrip("# ") + "\n"
            elif line == "":
                code_in_body += original_line + "\n"
            elif line.startswith("class "):
                # it is a class
                class_name = line.split("class ")[1].split(":")[0].split("()")[0].strip()

                temp_index = line_index + 1
                temp_code_block = ""
                base_line = lines[temp_index]
                indents_number = get_indent_number(base_line)
                while temp_index < len(lines):
                    temp_line = lines[temp_index]
                    new_indents_number = get_indent_number(temp_line)
                    if temp_line.strip()!="" and new_indents_number < indents_number:
                        break
                    temp_code_block += temp_line + "\n"
                    temp_index += 1
                line_index = temp_index - 1 #if the code block search stop on new code block, it should minus 1

                an_element = Python_Element_Instance()
                an_element.type = "class"
                an_element.name = class_name
                an_element.general_value = temp_code_block
            elif line.startswith("def "):
                # it is a function
                function_name = line.split("def ")[1].split("(")[0]
                function_code = ""
                end_of_a_function_new_line_counting = 0

                function_code += lines[line_index] + "\n" #try to save the function arguments
                temp_index = line_index + 1
                base_line = lines[temp_index]
                indents_number = get_indent_number(base_line)
                while temp_index < len(lines):
                    temp_line = lines[temp_index]

                    temp_indents_number = get_indent_number(temp_line)
                    if temp_line.strip()!="" and temp_indents_number < indents_number:
                        break

                    function_code += temp_line + "\n"
                    temp_index += 1

                line_index = temp_index - 1 #if the code block search stop on new code block, it should minus 1

                an_element = Python_Element_Instance()
                an_element.type = "function"
                an_element.name = function_name
                an_element.general_value = function_code
            elif " = " in line:
                key = line.split(" = ")[0].strip()
                value = line.split(" = ")[1].strip()
                if ("{" in value) or ("[" in value) or ("(" in value):
                    first_char = value[0] #need a function to get character from string
                    start_symbol = first_char
                    if start_symbol in ["{", "[", "("]:
                        if start_symbol == "{":
                            end_symbol = "}"
                        elif start_symbol == "[":
                            end_symbol = "]"
                        elif start_symbol == "(":
                            end_symbol = ")"
                        value = get_text_until_closed_symbol(lines[line_index:], start_symbol, end_symbol)
                        returned_element = process_code(variable_dict, value, level=level+1)
                        returned_element.name = key
                        # tip: the process_code() function must return the type of the value
                        if returned_element.type == "dict":
                            returned_element.translated_code_for_definition = returned_element.translated_code_for_definition.format(key=key)
                            returned_element.translated_code_for_real_part = returned_element.translated_code_for_real_part.format(key=key)
                            returned_element.translated_code = returned_element.translated_code.format(key=key)
                            element_list.append(returned_element)
                if "\n" in value:
                    jump_line_number = len(value.split("\n"))
                    line_index += jump_line_number

            line_index = line_index + 1

    if level == 0:
        # in global level, you have to define the variable twice, first assign the variable to NULL, then initilize the real value in main() function
        code_in_head = ""
        code_in_main_function = ""
        for element in element_list:
            if element.type == "dict":
                code_in_head += element.translated_code_for_definition + "\n"
                code_in_main_function += element.translated_code_for_real_part + "\n"
        code_in_main_function = set_indent(code_in_main_function, 1)
        global_code_template = """
#include "../y_python.h"
#include "../y_python_linux.h"

{code_in_head}

{code_in_body}

int main(int argument_number, char **argument_list) {{
{code_in_main_function}
    return 0;
}}
        """.format(code_in_head=code_in_head, code_in_body=code_in_body, code_in_main_function=code_in_main_function)
        default_element.translated_code = global_code_template
        return default_element

    return default_element

global_variable_dict = {}
root_element = process_code(global_variable_dict, code_text)
translated_code = root_element.translated_code

with open("./dist/y_python.c", "w", encoding="utf-8") as f:
    f.write(translated_code)
print(translated_code)
