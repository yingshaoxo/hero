# yingshaoxo: An advanced python interpreter

# To rewrite a python:
# 1. We need to parse python code into components or elements first, each element contains different information. For example: element name can be one of [str, float, int, bool, none, list, dict, ignore, variable_assignment, if_else, try_catch, while_break, function, return, function_call, class, class_instance, code, global_code].
# 2. Make sure the element has tree structure, I mean a class element may has a lot of functions elements inside, one element has many other child elements. We parse those elements from code, follow the order of 'top to bottom', 'big pattern to small pattern' or 'big template to small template'.
# 3. Then, based on the element tree, we can: 1. execute code one by one, just like an operator, where python code is the instructure or remote controller. 2. translate python code into c99 code

# If we want to convert a python code into c99 code, it has to have type information. Especially what type a function receive and returns, I mean, [str, float, int, bool, none, list, dict, any_class_definition]

# To test this script, translate itself into c99


class Python_Element_Instance():
    def __init__(self):
        # none, str, bool, int, float, list, dict, function(a_string_of_code_block), class, class_instance(propertys:dict{...variable_dict, ...functions.dict})
        self._type = "None"
        self._name = None # variable name, function name, class name
        self._value = None # in c, it is Ypython_General(), and most likely it is just pure text version of the original code
        self._children = [] # a list of Python_Element_Instance() representes the self._value
        self._parent = None
        self._information = {
            "_parsed_child_text": False,
            "_parsed_line_index": 0,
            "indent_string": ""
        }

    def __str__(self):
        print("______________")
        print(self._type)
        print(self._name)
        print("children number:", len(self._children))
        print(self._value)
        print("______________")
        for temp_element in self._children:
            print(temp_element)
        return ""


def get_indent_number(code: str) -> int:
    new_code = code.lstrip()
    return len(code) - len(new_code)

def get_indent(code: str) -> str:
    length = get_indent_number(code)
    return code[:length]

def set_indent(code: str, level: int=0) -> str:
    lines = code.split("\n")
    result_code = ""
    for line in lines:
        pure_line = line.strip()
        if pure_line != "":
            result_code += level*"    " + pure_line + "\n"
        else:
            result_code += "\n"
    return result_code

def get_text_until_closed_symbol(lines: list, start_symbol: str, end_symbol: str) -> str:
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

def pre_process_comments(text_code: str) -> str:
    new_code = ""
    lines = text_code.split("\n")
    for line in lines:
        original_line = line
        if "#" in line and not line.strip().startswith("#"):
            has_comment = False
            comment = ""
            for char in reversed(line):
                comment += char
                if char == "#":
                    has_comment = True
                    break

            if has_comment == True:
                comment = str("".join(list(reversed(comment))))
                line = line.split(comment)[0]
                new_code += line + "\n"
                new_code += get_indent(original_line) + comment + "\n"
            else:
                new_code += line + "\n"
        else:
            new_code += line + "\n"
    return new_code


def parse_code_by_char(text_code: str, just_return_one_element: bool=False) -> Python_Element_Instance:
    # similar to eval() in python, which takes '1 + (1 * 2)', output '3'
    # it should also parse dict, list, string, float, int, bool, None
    a_element = Python_Element_Instance()
    a_element._type = "ignore"

    text_code = text_code.strip()

    if text_code == "":
        return a_element
    elif text_code == "None":
        a_element._type = "None"
    elif text_code.startswith('"') and text_code.endswith('"'):
        # it is a string
        a_element._type = "str"
        a_element._value = text_code[1:-1]
    elif text_code.startswith("'") and text_code.endswith("'"):
        # it is a string
        a_element._type = "str"
        a_element._value = text_code[1:-1]
    elif text_code.replace(".","").isdigit():
        # it is a number
        # is_digit: use xx.strip("0123456789.") == "" could also do the job
        if "." in text_code:
            # it is float
            a_element._type = "float"
            a_element._value = text_code
        else:
            # it is int
            a_element._type = "int"
            a_element._value = text_code
    elif text_code == "True" or text_code == "False":
        # it is bool
        a_element._type = "bool"
        if text_code == "True":
            a_element._value = "True"
        else:
            a_element._value = "False"
    elif text_code.startswith('[') and text_code.endswith(']'):
        # it is list
        a_element._type = "list"
        new_text_code = text_code[1:-1]
        if new_text_code.strip() != "":
            for element_string in new_text_code.split(","):
                a_element._children.append(parse_code_by_char(element_string))
        a_element._value = text_code
    elif text_code.startswith('{') and text_code.endswith('}'):
        # it is dict
        a_element._type = "dict"
        a_element._value = text_code
    else:
        # unknow, treat it as string
        a_element._type = "str"
        a_element._value = text_code
        #an_element = Python_Element_Instance()
        #an_element._type = "ignore"
        #an_element._name = ""
        #an_element._value = original_line
    return a_element


def parse_code(text_code: str, just_return_one_element: bool=False, global_code=False) -> Python_Element_Instance:
    # return an Python_Element_Instance
    default_element = Python_Element_Instance()
    if global_code == True:
        default_element._type = "global_code"
    else:
        default_element._type = "code"
    default_element._name = ""
    default_element._value = text_code

    if text_code == None:
        default_element._type = "None"
        return default_element

    lines = text_code.split("\n")
    line_index = 0
    while line_index < len(lines):
        line = lines[line_index]
        original_line = line
        line = line.strip()

        if line == "":
            an_element = Python_Element_Instance()
            an_element._type = "ignore"
            an_element._name = ""
            an_element._value = ""
        elif line.startswith("#"):
            an_element = Python_Element_Instance()
            an_element._type = "comment"
            an_element._name = ""
            an_element._value = original_line
        elif line.startswith("class "):
            # it is a class
            class_name = line.split("class ")[1].split(":")[0].split("()")[0].strip()

            temp_code_block = ""
            temp_code_block += lines[line_index] + "\n" #try to save the class arguments
            temp_index = line_index + 1
            base_line = lines[temp_index]
            indents_number = get_indent_number(base_line)
            while temp_index < len(lines):
                temp_line = lines[temp_index]
                new_indents_number = get_indent_number(temp_line)
                if temp_line.strip() != "" and new_indents_number < indents_number:
                    break
                temp_code_block += temp_line + "\n"
                temp_index += 1
            line_index = temp_index - 1 #if the code block search stop on new code block, it should minus 1

            an_element = Python_Element_Instance()
            an_element._type = "class"
            an_element._name = class_name
            an_element._value = temp_code_block.strip()

            children = []
            functions_code_block = "\n".join(temp_code_block.split("\n")[1:])
            while True:
                temp_element = parse_code(functions_code_block, just_return_one_element=True)
                if temp_element._type != "ignore":
                    children.append(temp_element)
                new_index_for_the_code_that_did_not_get_parsed = temp_element._information["_parsed_line_index"]
                functions_code_block = "\n".join(functions_code_block.split("\n")[new_index_for_the_code_that_did_not_get_parsed:])
                if functions_code_block.strip() == "":
                    break

            an_element._children = children
            an_element._information["_parsed_child_text"] = True
            an_element._information["_pure_class_code"] = "\n".join(an_element._value.split("\n")[1:])
        elif line.startswith("def "):
            # it is a function
            function_name = line.split("def ")[1].split("(")[0]
            function_code = ""
            end_of_a_function_new_line_counting = 0

            function_head_line = lines[line_index]
            function_code += function_head_line + "\n" #try to save the function arguments
            temp_index = line_index + 1
            base_line = lines[temp_index]
            indents_number = get_indent_number(base_line)
            while temp_index < len(lines):
                temp_line = lines[temp_index]

                # handle multiple line string comment
                if '= """' in temp_line:
                    function_code += temp_line + "\n"
                    temp_index2 = temp_index + 1
                    while temp_index2 < len(lines):
                        temp_line2 = lines[temp_index2]
                        if '"""' in temp_line2:
                            function_code += temp_line2 + "\n"
                            temp_index = temp_index2 + 1
                            break
                        else:
                            function_code += temp_line2 + "\n"
                        temp_index2 += 1
                    continue

                temp_indents_number = get_indent_number(temp_line)
                if temp_line.strip() != "" and temp_indents_number < indents_number:
                    break

                function_code += temp_line + "\n"
                temp_index += 1

            line_index = temp_index - 1 #if the code block search stop on new code block, it should minus 1
            an_element = Python_Element_Instance()
            an_element._type = "function"
            an_element._name = function_name
            function_code = function_code.strip()
            an_element._value = function_code
            an_element._information["_parsed_child_text"] = False

            if " -> " in function_head_line:
                return_type = function_head_line.split(" -> ")[1].split(":")[0]
                an_element._information["_return_type"] = return_type.strip()
            an_element._information["_parameter_string"] = function_head_line.split("(")[1].split(")")[0]
            an_element._information["_pure_function_code"] = "\n".join(function_code.split("\n")[1:])

        elif " = " in line and line.count(" = ") == 1:
            # variable assignment
            key, value = line.split(" = ")
            key, value = key.strip(), value.strip()

            an_element = Python_Element_Instance()
            an_element._type = "variable_assignment"
            an_element._name = key
            an_element._value = value
            # if this is a function call, it has to be false, later in process_code(), if parsed_child_text == False, you have to do a parse first to get new children list
            an_element._information["_parsed_child_text"] = True

            #if "." in key:
            #    # class instance related assignment operation
            #    pass

            # normal variable related assignment operation
            if value.endswith(")"):
                # it is come from a function call
                an_element._information["_parsed_child_text"] = False
                #an_element = handle_function_call(variable_dict, value, process)
            elif value.startswith('"""'):
                # it is a raw string, """could have no leading space in next line"""
                long_text = ""
                temp_index = line_index + 1
                while temp_index < len(lines):
                    temp_line = lines[temp_index]
                    if temp_line.endswith('"""'):
                        break
                    long_text += temp_line + "\n"
                    temp_index += 1
                line_index = temp_index

                an_element._value = long_text

                an_element_2 = Python_Element_Instance()
                an_element_2._type = "str"
                an_element_2._value = long_text

                an_element._children.append(an_element_2)
            elif value.startswith("'''"):
                # it is a raw string, '''could have no leading space in next line'''
                long_text = ""
                temp_index = line_index + 1
                while temp_index < len(lines):
                    temp_line = lines[temp_index]
                    if temp_line.endswith("'''"):
                        break
                    long_text += temp_line + "\n"
                    temp_index += 1
                line_index = temp_index

                an_element._value = long_text

                an_element_2 = Python_Element_Instance()
                an_element_2._type = "str"
                an_element_2._value = long_text

                an_element._children.append(an_element_2)
            else:
                # normal value
                start_symbol = value[0]
                if start_symbol in ["{", "["]:
                    if start_symbol == "{":
                        end_symbol = "}"
                    elif start_symbol == "[":
                        end_symbol = "]"
                    value = get_text_until_closed_symbol(lines[line_index:], start_symbol, end_symbol)
                    if "\n" in value:
                        jump_line_number = len(value.split("\n"))
                        line_index += jump_line_number
                    an_element._value = value
                    an_element._children.append(parse_code_by_char(value))
                else:
                    # should be a value that can be get from eval() function
                    an_element._children.append(parse_code_by_char(value))

        elif line.endswith(")"):
            # it is function call
            an_element = Python_Element_Instance()
            an_element._type = "function_call"
            an_element._value = line
            function_name = line.split("(")[0]
            function_arguments = line.split("(")[1].split(")")[0]
            if "." in function_name:
                class_instance_name, function_name = line.split(".")
                an_element._information["is_class_function"] = True
                an_element._information["class_instance_name"] = class_instance_name
            an_element._information["function_name"] = function_name
            an_element._information["function_arguments"] = function_arguments
        else:
            an_element = parse_code_by_char(original_line)

        an_element._information["indent_string"] = get_indent(original_line)

        if just_return_one_element == True:
            an_element._information["_parsed_line_index"] = line_index + 1
            return an_element
        else:
            if an_element._type != "ignore":
                default_element._children.append(an_element)

        line_index += 1

    return default_element


def process_code(variable_dict: dict, an_element: Python_Element_Instance=None, code: str=None):
    if an_element == None and code == None:
        return

    if code != None:
        an_element = parse_code(code)


def _to_c99_type_from_python_type(type_string: str) -> str:
    #c99_type_translate_dict = {
    #    "None": "Type_Ypython_None",
    #    "str": "Type_Ypython_String",
    #    "int": "Type_Ypython_Int",
    #    "float": "Type_Ypython_Float",
    #    "bool": "Type_Ypython_Bool",
    #    "list": "Type_Ypython_List",
    #    "dict": "Type_Ypython_Dict",
    #}
    #result = c99_type_translate_dict.get(type_string)
    #if result == None:
    #    return "Type_" + type_string
    #else:
    #    return result
    if type_string not in ["None", "str", "int", "float", "list", "dict"]:
        return "Type_" + type_string
    else:
        return "Type_Ypython_General"

def _to_c99_value_from_python_value(value_string: str, information_dict: dict) -> str:
    an_element = parse_code(value_string)

    if len(an_element._children) >= 1:
        an_element = an_element._children[0]

    if "variable_name" not in information_dict:
        if an_element._type == "None":
            return "Ypython_None()"
        elif an_element._type == "str":
            return "Ypython_String(" + '"' + an_element._value + '"' + ")"
        elif an_element._type == "int":
            return "Ypython_Int(" + an_element._value + ")"
        elif an_element._type == "float":
            return "Ypython_Float(" + an_element._value + ")"
        elif an_element._type == "dict":
            return "Ypython_Dict("+ "" + ")"
    elif ("variable_name" in information_dict) and ("use_general_value" in information_dict):
        variable_name = information_dict["variable_name"]

        translated_code = ""
        if 1 == 2:
            pass
        elif an_element._type == "list":
            temp_list_variable_name = "temp_{variable_name}_{level}_{count}".format(variable_name=variable_name, level=information_dict["level"], count=information_dict["number_count"])
            translated_code += "Type_Ypython_List *{temp_list_variable_name} = Ypython_List();\n".format(temp_list_variable_name=temp_list_variable_name)
            information_dict["level"] = information_dict["level"] + 1
            for child in an_element._children:
                information_dict["number_count"] = information_dict["number_count"] + 1
                translated_code += "{temp_list_variable_name}->function_append({temp_list_variable_name}, {new_value});\n".format(
                    temp_list_variable_name=temp_list_variable_name,
                    new_value=_to_c99_value_from_python_value(child._value, {})
                )
            translated_code += information_dict["indents_string"] + "Type_Ypython_General *{variable_name} = ypython_create_a_general_variable({temp_list_variable_name});".format(variable_name=variable_name, temp_list_variable_name=temp_list_variable_name)
        elif an_element._type == "dict":
            part_1 = "Ypython_Dict(" + ")"
            translated_code = "Type_Ypython_General *{variable_name} = ypython_create_a_general_variable({basic_value});".format(variable_name=variable_name, basic_value=part_1)
        else:
            basic_value = _to_c99_value_from_python_value(an_element._value, {})
            translated_code = "Type_Ypython_General *{variable_name} = ypython_create_a_general_variable({basic_value});".format(variable_name=variable_name, basic_value=basic_value)
        return translated_code
    elif ("variable_name" in information_dict) and ("use_general_value" not in information_dict):
        variable_name = information_dict["variable_name"]
        if 1 == 2:
            pass
        elif an_element._type == "dict":
            part_1 = "Ypython_Dict(" + ")"
            translated_code = "Type_Ypython_Dict *{variable_name} = {basic_value};".format(variable_name=variable_name, basic_value=part_1)
        return translated_code
    else:
        translated_code = ""
        return translated_code


def tranalste_to_c99(a_python_element: Python_Element_Instance) -> str:
    # you already parsed the class and function into objects
    # then you have to convert those objects into c99 code, just a big strucure for class and function

    translated_code = ""

    if a_python_element._type == "global_code":
        translated_code += """
#include "../y_python.h"
#include "../y_python_linux.h"
"""

        for child in a_python_element._children:
            translated_code += tranalste_to_c99(child)

        translated_code += """
int main(int argument_number, char **argument_list) {
    //{code_in_main_function}
    return 0;
}
"""
    elif a_python_element._type == "function" and "_return_type" in a_python_element._information:
        # you have to know the return type and paramater type
        function_code = """
{return_type} *{function_name}({parameter_string}) {{
{function_code}
}}
"""
        _return_type = a_python_element._information["_return_type"]
        return_type = _to_c99_type_from_python_type(_return_type)
        translated_code += function_code.format(
            return_type=return_type,
            function_name=a_python_element._name,
            parameter_string=a_python_element._information["_parameter_string"],
            function_code=a_python_element._information["_pure_function_code"]
        )
    elif a_python_element._type == "class":
        class_code = """
typedef struct Type_{class_name} Type_{class_name};
struct Type_{class_name} {{
{property_definition}

{functions_definition}
}};

{functions_code_string}

Type_{class_name} *{class_name}() {{
    Type_{class_name} *new_element_instance;
    new_element_instance = (Type_{class_name} *)malloc(sizeof(Type_{class_name}));

{property_initiation}

{functions_initiation}

    return new_element_instance;
}}
"""
        property_definition_string = ""
        property_initiation_string = ""
        indents_string = ""
        if len(a_python_element._children) >= 1:
            the_init_function_element = a_python_element._children[0]
            indents_string = the_init_function_element._information["indent_string"]
            if the_init_function_element._name == "__init__":
                # this class object has some propertys to initialize
                code_object = parse_code(the_init_function_element._information["_pure_function_code"])
                for child in code_object._children:
                    if child._type == "variable_assignment":
                        key, value = child._name, child._value.replace("\n", "")
                        if "self." in key:
                            # put it into class structure
                            name = key.split("self.")[1]
                            the_type = _to_c99_type_from_python_type(child._children[0]._type)
                            #indents_string = child._information["indent_string"]
                            property_definition_string += indents_string + the_type + " *" + name + ";" + "\n"
                            # put it into class initialization function
                            if len(child._children) == 0:
                                pass
                            else:
                                property_initiation_string += indents_string + _to_c99_value_from_python_value(value.strip(), {"variable_name": name, "level": 0, "number_count": 0, "indents_string": indents_string}) + "\n"

                                property_initiation_string += indents_string + "new_element_instance->" + name + " = " + name + ";\n\n"

        functions_code_string = ""
        for child in a_python_element._children:
            if child._name != "__init__":
                function_name, function_content = child._name, child._value
                temp_string = """
{return_type} *Type_{class_name}_{function_name}(Type_{class_name} *self) {{
}}
""".strip()
                functions_code_string += temp_string.format(
                    return_type="void",
                    class_name=a_python_element._name,
                    function_name=child._name
                )

        functions_definition = ""
        functions_initiation = ""
        for child in a_python_element._children:
            if child._name != "__init__":
                function_name, function_content = child._name, child._value

                definition_temp_string = """
{return_type} *(*{function_name})(Type_{class_name} *self);
""".strip()
                functions_definition += indents_string + definition_temp_string.format(
                    return_type="void",
                    class_name=a_python_element._name,
                    function_name=child._name
                )

                initiation_temp_string = """
new_element_instance->{function_name} = &Type_{class_name}_{function_name};
""".strip()
                functions_initiation += indents_string + initiation_temp_string.format(
                    class_name=a_python_element._name,
                    function_name=child._name
                )

        translated_code += class_code.format(
            class_name = a_python_element._name,
            property_definition = property_definition_string,
            property_initiation = property_initiation_string,
            functions_code_string = functions_code_string,
            functions_definition = functions_definition,
            functions_initiation = functions_initiation
        )

        #print(translated_code)
    elif a_python_element._type == "function_call":
        if "is_class_function" in a_python_element._information:
            translated_code += a_python_element._information["class_instance_name"] + "->" + a_python_element._information["function_name"] + "(" + a_python_element._information["function_arguments"] + ")" + ";"
        else:
            translated_code += a_python_element._information["function_name"] + "(" + a_python_element._information["function_arguments"] + ")" + ";"
    elif a_python_element._type == "variable_assignment":
        indents_string = a_python_element._information["indent_string"]
        key, value = a_python_element._name, a_python_element._value
        translated_code += _to_c99_value_from_python_value(value.strip(), {"variable_name": key, "level": 0, "number_count": 0, "indents_string": indents_string}) + "\n"
    elif len(a_python_element._children) != 0:
        for child in a_python_element._children:
            translated_code += tranalste_to_c99(child)


    return translated_code


def tranalste_to_golang(a_python_element: Python_Element_Instance) -> str:
    # it is just a example, your server should only use one architecture, which is the computer you can build by yourself.
    # but the client software code changes very quick as their owner intend to make it hard for third party software developer to do developement
    pass


def test_by_doing_a_self_compile():
    with open("python_to_c99.py", encoding="utf-8") as f:
        a_py_file_text = f.read()
    a_py_file_text = pre_process_comments(a_py_file_text)

    global_variable_dict = {}
    global_code_object = parse_code(a_py_file_text, global_code=True)

    import os
    os.system("clear")

    print(global_code_object)
    final_code = tranalste_to_c99(global_code_object)
    print(final_code)


def real_time_c99_translator_shell():
    import os

    from auto_everything.terminal import Terminal_User_Interface, Terminal
    terminal_user_interface = Terminal_User_Interface()
    terminal = Terminal()

    while True:
        os.system("clear")
        input_python_code = terminal_user_interface.input_box("You can paste your python code in here. Then save it to let the translation begin:", with_new_line=True).strip()
        os.system("clear")

        if input_python_code == "":
            break

        input_python_code = pre_process_comments(input_python_code)

        global_variable_dict = {}
        global_code_object = parse_code(input_python_code, global_code=False)
        print(global_code_object)
        final_code = tranalste_to_c99(global_code_object).strip()
        #print(terminal.run_command("xclip -selection clipboard -o"))
        terminal.run('echo -e "{text}" | xclip -selection clipboard'.format(text=final_code.replace('\n', '\\n')), use_os_system=True)

        #os.system("clear")
        print(final_code)

        answer = input("\nTranslate next one? (y/n)").strip()
        if answer == "n":
            break


real_time_c99_translator_shell()
#test_by_doing_a_self_compile()
