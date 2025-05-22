# yingshaoxo: An advanced python interpreter

# To rewrite a python:
# 1. We need to parse python code into components or elements first, each element contains different information. For example: element name can be one of [string, float, int, bool, none, list, dict, ignore, variable_assignment, if_else, try_catch, while_break, function, return, function_call, class, class_instance, global_code].
# 2. Make sure the element has tree structure, I mean a class element may has a lot of functions elements inside, one element has many other child elements. We parse those elements from code, follow the order of 'top to bottom', 'big pattern to small pattern' or 'big template to small template'.
# 3. Then, based on the element tree, we can: 1. execute code one by one, just like an operator, where python code is the instructure or remote controller. 2. translate python code into c99 code

# If we want to convert a python code into c99 code, it has to have type information. Especially what type a function receive and returns, I mean, [string, float, int, bool, none, list, dict, any_class_definition]


class Python_Element_Instance():
    def __init__(self):
        # none, string, bool, int, float, list, dict, function(a_string_of_code_block), class, class_instance(propertys:dict{...variable_dict, ...functions.dict})
        self._type = "None"
        self._name = None # variable name, function name, class name
        self._value = None # in c, it is Ypython_General(), and most likely it is just pure text version of the original code
        self._children = [] # a list of Python_Element_Instance() representes the self._value
        self._parent = None
        self._information = {
            "_parsed_child_text": False,
            "_parsed_line_index": 0,
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


def parse_code_by_char(text_code: str, just_return_one_element: bool=False) -> Python_Element_Instance:
    # similar to eval() in python, which takes '1 + (1 * 2)', output '3'
    # it should also parse dict, list, string, float, int, bool, None

    a_element = Python_Element_Instance()
    a_element._type = "ignore"

    text_code = text_code.strip()

    if text_code == "":
        return default_element
    elif text_code.startswith('"') and text_code.endswith('"'):
        # it is a string
        a_element._type = "string"
        a_element._value = text_code[1:-1]
    elif text_code.startswith("'") and text_code.endswith("'"):
        # it is a string
        a_element._type = "string"
        a_element._value = text_code[1:-1]
    elif text_code.replace(".","").isdigit():
        # it is a number
        # is_digit: use xx.strip("0123456789.") == "" could also do the job
        if "." in text_code:
            # it is float
            a_element._type = "float"
            a_element._value = float(text_code)
        else:
            # it is int
            a_element._type = "int"
            a_element._value = int(text_code)
    elif text_code == "True" or text_code == "False":
        # it is bool
        a_element._type = "bool"
        if text_code == "True":
            a_element._value = True
        else:
            a_element._value = False
    elif text_code.startswith('[') and text_code.endswith(']'):
        # it is list
        a_element._type = "list"
        values = text_code[1:-1].split(", ")
        values = [one for one in values]
        a_element._value = str(values)
    elif text_code.startswith('{') and text_code.endswith('}'):
        # it is dict
        a_element._type = "dict"
        text_code = text_code[1:-1]
        if text_code.strip() != "":
            values = text_code.split(", ") # there may have a bug when a list is in the dict
            values = {one.split(": ")[0].strip(): one.split(":")[1].strip() for one in values}
        else:
            values = {} # there may have a bug when a list is in the dict
        a_element._value = str(values)
    else:
        # unknow, treat it as string
        a_element._type = "string"
        a_element._value = text_code
    return a_element


def parse_code(text_code: str, just_return_one_element: bool=False) -> Python_Element_Instance:
    # return an Python_Element_Instance
    default_element = Python_Element_Instance()
    default_element._type = "global_code"
    default_element._name = ""
    default_element._value = text_code

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

            if "." in key:
                # class instance related assignment operation
                pass
            else:
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
                    an_element_2._type = "string"
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
                    an_element_2._type = "string"
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
        else:
            an_element = Python_Element_Instance()
            an_element._type = "ignore"
            an_element._name = ""
            an_element._value = original_line

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


def tranalste_to_c99(a_python_element: Python_Element_Instance) -> str:
    # you already parsed the class and function into objects
    # then you have to convert those objects into c99 code, just a big strucure for class and function
    type_translate_dict = {
        "None": "Type_Ypython_None",
        "str": "Type_Ypython_String",
        "int": "Type_Ypython_Int",
        "float": "Type_Ypython_Float",
        "bool": "Type_Ypython_Bool",
        "list": "Type_Ypython_List",
        "dict": "Type_Ypython_Dict",
    }

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
        if _return_type in type_translate_dict:
            return_type = type_translate_dict.get(_return_type)
        else:
            return_type = "Type_" + _return_type
        translated_code += function_code.format(
            return_type=return_type,
            function_name=a_python_element._name,
            parameter_string=a_python_element._information["_parameter_string"],
            function_code=a_python_element._information["_pure_function_code"]
        )

    return translated_code


def tranalste_to_golang(a_python_element: Python_Element_Instance) -> str:
    # it is just a example, your server should only use one architecture, which is the computer you can build by yourself.
    # but the client software code changes very quick as their owner intend to make it hard for third party software developer to do developement
    pass



with open("y_python_advanced.py", encoding="utf-8") as f:
    a_py_file_text = f.read()

global_variable_dict = {}
global_code_object = parse_code(a_py_file_text)

import os
os.system("clear")

#print(global_code_object)
print(tranalste_to_c99(global_code_object))
