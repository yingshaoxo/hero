# yingshaoxo: A demo python parser, definately have bugs, it is just fake code, rewrite it carefully to prevent bugs

# normally in python you get this dict by using dir()
global_variable_dict = {
    "__built_in_s__": ["type"]
}

class Python_Element_Instance():
    def __init__(self):
        # none, string, bool, int, float, list, dict, function(a_string_of_code_block), class(a_dict{propertys:dict(), functions:dict()})
        self.type = "None"
        self.name = None # variable name, function name, class name
        self.general_value = None # in c, it is Ypython_General()

def get_python_element_instance(variable_dict, a_variable_name_or_raw_value):
    global global_variable_dict

    if (a_variable_name_or_raw_value not in variable_dict) and (a_variable_name_or_raw_value not in global_variable_dict):
        # may detect if its a number or bool or string or ...
        a_element = Python_Element_Instance()
        if a_variable_name_or_raw_value.startswith('"') and a_variable_name_or_raw_value.endswith('"'):
            # it is a string
            a_element.type = "string"
            a_element.general_value = a_variable_name_or_raw_value[1:-1]
        elif a_variable_name_or_raw_value.replace(".","").isdigit():
            # it is a number
            if "." in a_variable_name_or_raw_value:
                # it is float
                a_element.type = "float"
                a_element.general_value = float(a_variable_name_or_raw_value)
            else:
                # it is int
                a_element.type = "int"
                a_element.general_value = int(a_variable_name_or_raw_value)
        elif a_variable_name_or_raw_value == "True" or a_variable_name_or_raw_value == "False":
            # it is bool
            a_element.type = "bool"
            if a_variable_name_or_raw_value == "True":
                a_element.general_value = True
            else:
                a_element.general_value = False
        elif a_variable_name_or_raw_value.startswith('[') and a_variable_name_or_raw_value.endswith(']'):
            # it is list
            a_element.type = "list"
            values = a_variable_name_or_raw_value[1:-1].split(", ")
            values = [get_python_element_instance(variable_dict, one) for one in values]
            a_element.general_value = values
        elif a_variable_name_or_raw_value.startswith('{') and a_variable_name_or_raw_value.endswith('}'):
            # it is dict
            a_element.type = "dict"
            values = a_variable_name_or_raw_value[1:-1].split(", ") # there may have a bug when a list is in the dict
            values = {one.split(": ")[0].strip(): get_python_element_instance(variable_dict, one.split(":")[1].strip()) for one in values}
            a_element.general_value = values
        else:
            # unknow, treat it as string
            a_element.type = "string"
            a_element.general_value = "Error: no variable called '" + str(a_variable_name_or_raw_value) + "'"
        return a_element
    else:
        if a_variable_name_or_raw_value in variable_dict:
            return variable_dict[a_variable_name_or_raw_value]
        elif a_variable_name_or_raw_value in global_variable_dict:
            return global_variable_dict[a_variable_name_or_raw_value]

def handle_one_line_operations(variable_dict, one_line_code):
    #return eval(one_line_code)
    if " + " in one_line_code:
        parts = one_line_code.split(" + ")
        parts = [get_python_element_instance(variable_dict, one) for one in parts]
        value = parts[0].general_value
        for one_value in parts[1:]:
            value += one_value.general_value

        an_element = Python_Element_Instance()
        an_element.type = "string"
        an_element.general_value = value

        return an_element
    elif " - " in one_line_code:
        pass
    elif " * " in one_line_code:
        pass
    elif " / " in one_line_code:
        pass
    elif " > " in one_line_code:
        pass
    elif " >= " in one_line_code:
        pass
    elif " < " in one_line_code:
        parts = one_line_code.split(" < ")

        an_element = Python_Element_Instance()
        an_element.type = "bool"

        if len(parts) == 2:
            an_element.general_value = get_python_element_instance(variable_dict, parts[0]).general_value < get_python_element_instance(variable_dict, parts[1]).general_value
        else:
            an_element.general_value = False

        return an_element
    elif " <= " in one_line_code:
        pass
    elif " == " in one_line_code:
        parts = one_line_code.split(" == ")

        an_element = Python_Element_Instance()
        an_element.type = "bool"

        if len(parts) == 2:
            an_element.general_value = get_python_element_instance(variable_dict, parts[0]).general_value == get_python_element_instance(variable_dict, parts[1]).general_value
        else:
            an_element.general_value = False

        return an_element
    elif " != " in one_line_code:
        pass
    #elif " and " in one_line_code: # cause parsing error, need to find a way to solve this problem
    #    pass
    #elif " or " in one_line_code:
    #    pass
    else:
        return get_python_element_instance(variable_dict, one_line_code)

def handle_function_call(variable_dict, one_line_code, process_function):
    global global_variable_dict

    line = one_line_code

    function_name = line.split("(")[0]
    function_arguments = line.split("(")[1].split(")")[0].strip()

    if function_arguments != "" and ", " in function_arguments:
        arguments_are_variable_dict = {one.split("=")[0]: handle_one_line_operations(variable_dict, one.split("=")[1]) for one in function_arguments.split(", ")}
        # how to handle list arguments function(1,2,3) than function(a=b) is a problem here.
    else:
        arguments_are_variable_dict = {}

    if (function_name in variable_dict) or (function_name in global_variable_dict):
        if function_name in variable_dict:
            an_element = variable_dict[function_name]
        elif function_name in global_variable_dict:
            an_element = global_variable_dict[function_name]
        if an_element.type == "function":
            local_dict_for_a_function = variable_dict.copy()
            local_dict_for_a_function.update(arguments_are_variable_dict)
            return process_function(local_dict_for_a_function, "\n".join(an_element.general_value.split("\n")[1:]))
    elif function_name in global_variable_dict["__built_in_s__"]:
        if function_name == "type":
            an_element = Python_Element_Instance()
            an_element.type = "string"
            an_element.general_value = handle_one_line_operations(variable_dict, function_arguments).type
            return an_element
    else:
        print("Error: no function called '" + function_name + "'")

def general_print(an_element, end="\n"):
    if an_element.type == "list":
        print("[", end="")
        for index, temp_element in enumerate(an_element.general_value):
            general_print(temp_element, end="")
            if index != len(an_element.general_value)-1:
                print(", ", end="")
        print("]", end="\n")
    elif an_element.type == "dict":
        for temp_element_key, temp_element_value in an_element.general_value.items():
            print(temp_element_key, end="")
            print(": ", end="")
            general_print(temp_element_value)
    else:
        print(an_element.general_value, end=end)

def process(variable_dict, text_code):
    # handle code, mainly just for codes inside of a function
    lines = text_code.split("\n")
    line_index = 0
    while line_index < len(lines):
        line = lines[line_index]
        if line.strip().startswith("#"):
            pass
        elif line.strip().startswith("if "):
            if_line = line

            temp_index = line_index + 1
            temp_code_block = ""
            base_line = lines[temp_index]
            indents_number = len(base_line) - len(base_line.lstrip())
            while temp_index < len(lines):
                temp_line = lines[temp_index]
                new_indents_number = len(temp_line) - len(temp_line.lstrip())
                if temp_line.strip()!="" and new_indents_number < indents_number:
                    break
                temp_code_block += temp_line + "\n"
                temp_index += 1
            line_index = temp_index - 1 #if the code block search stop on new code block, it should minus 1

            verifying = if_line.split("if ")[1].split(":")[0]
            verifying = handle_one_line_operations(variable_dict, verifying)

            if verifying.type == "bool":
                if verifying.general_value == True:
                    process(variable_dict, temp_code_block)
        elif line.strip().startswith("while "):
            while_line = line

            temp_index = line_index + 1
            temp_code_block = ""
            base_line = lines[temp_index]
            indents_number = len(base_line) - len(base_line.lstrip())
            while temp_index < len(lines):
                temp_line = lines[temp_index]
                new_indents_number = len(temp_line) - len(temp_line.lstrip())
                if temp_line.strip()!="" and new_indents_number < indents_number:
                    break
                temp_code_block += temp_line + "\n"
                temp_index += 1
            line_index = temp_index - 1 #if the code block search stop on new code block, it should minus 1

            an_element = Python_Element_Instance()
            an_element.type = "bool"
            an_element.general_value = False
            variable_dict["___force_break_while_loop___"] = an_element
            while True:
                verifying = while_line.split("while ")[1].split(":")[0]
                verifying = handle_one_line_operations(variable_dict, verifying)

                if verifying.type == "bool":
                    if verifying.general_value == True:
                        process(variable_dict, temp_code_block)
                        if variable_dict["___force_break_while_loop___"].general_value == True:
                            break
                        else:
                            continue
                break
        elif " = " in line:
            # we save that variable to global variable dict
            key, value = line.split(" = ")
            key, value = key.strip(), value.strip()
            if value.endswith(")"):
                # it is a function call
                an_element = handle_function_call(variable_dict, value, process)
            elif value.startswith('"""'):
                # it is a raw string, """could have no leading space in next line"""
                long_text = ""
                temp_index = line_index + 1
                while temp_index < len(lines):
                    temp_line = lines[temp_index]
                    long_text += temp_line + "\n"
                    if temp_line.endswith('"""'):
                        break
                    temp_index += 1
                line_index = temp_index
                an_element = Python_Element_Instance()
                an_element.type = "string"
                an_element.general_value = long_text[:-5]
            else:
                # normal value
                an_element = handle_one_line_operations(variable_dict, value)
            an_element.name = key
            variable_dict[key] = an_element
        elif "print(" in line:
            key = line.split("print(")[1].split(")")[0]
            value_instance = handle_one_line_operations(variable_dict, key)
            general_print(value_instance)
        elif line.startswith("def "):
            # it is a function, we should save it in somewhere
            function_name = line.split("def ")[1].split("(")[0]
            function_code = ""
            end_of_a_function_new_line_counting = 0

            #temp_index = line_index + 1 #try to save the function arguments, so not plus one
            temp_index = line_index
            base_line = lines[temp_index+1]
            indents_number = len(base_line) - len(base_line.lstrip())
            while temp_index < len(lines):
                temp_line = lines[temp_index]
                function_code += temp_line + "\n"
                temp_index += 1

                temp_indents_number = len(lines[temp_index]) - len(lines[temp_index].lstrip())
                if lines[temp_index].strip()!="" and temp_indents_number < indents_number:
                    break
            line_index = temp_index - 1 #if the code block search stop on new code block, it should minus 1

            an_element = Python_Element_Instance()
            an_element.type = "function"
            an_element.name = function_name
            an_element.general_value = function_code
            variable_dict[function_name] = an_element
        elif (not line.startswith("def ")) and "(" in line and line.endswith(")"):
            # it is calling a function
            handle_function_call(variable_dict, line, process)
        elif line.strip().startswith("return "):
            return_variable_name = line.split("return ")[1]
            return_variable_name = handle_one_line_operations(variable_dict, return_variable_name)
            return return_variable_name
        elif line.strip() == "break":
            an_element = Python_Element_Instance()
            an_element.type = "bool"
            an_element.general_value = True
            variable_dict["___force_break_while_loop___"] = an_element
            return Python_Element_Instance()

        line_index += 1

a_py_file_text = '''
parent_variable = "parent"
print(parent_variable)

def a_function_1():
    a_child_variable = "whatever"
    print(parent_variable)
    print(a_child_variable)

a_function_1()
print(a_child_variable)

def a_function_2(temp_2):
    temp_1 = " you say"
    a_child_variable2 = "whatever" + temp_1
    print(a_child_variable2)
    print(temp_2)

a_function_2(temp_2=" haha ", temp_1="hi")
print("is" + " right")

a_dict = {a: 3}
print(a_dict)

a_list = ["god", "yingshaoxo"]
print(a_list)

print(2 + 3)

a_type = type(2)
print(a_type)

def a_function_3(number_1, number_2):
    return number_1 + number_2

result1 = a_function_3(number_1=6, number_2=7)
print(result1)

long_text = """
hi you,
    dear.
"""

print(long_text)


a2 = 1
b2 = 1
print(a2 == b2)
if a2 == b2:
    print("a2 and b2 is equal")

while a2 < 7:
    print(a2)
    a2 = a2 + 1
    if a2 == 4:
        break
'''

process(global_variable_dict, a_py_file_text)
