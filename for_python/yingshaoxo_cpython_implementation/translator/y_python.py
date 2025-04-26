# yingshaoxo: A demo python parser, definately have bugs, it is just fake code, rewrite it carefully to prevent bugs

global_variable_dict = { } # normally in python you get this dict by using dir()

class Python_Element_Instance():
    def __init__(self):
        # none, string, bool, int, float, list, dict, function(a_string_of_code_block), class(a_dict{propertys:dict(), functions:dict()})
        self.type = "None"
        self.name = None # variable name, function name, class name
        self.general_value = None

def get_python_element_instance(a_variable_name_or_raw_value):
    if a_variable_name_or_raw_value not in global_variable_dict:
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
            values = a_variable_name_or_raw_value[1:-1].split(",")
            values = [get_python_element_instance(one) for one in values]
            a_element.general_value = values
        elif a_variable_name_or_raw_value.startswith('{') and a_variable_name_or_raw_value.endswith('}'):
            # it is dict
            a_element.type = "dict"
            values = a_variable_name_or_raw_value[1:-1].split(",") # there may have a bug when a list is in the dict
            values = {one.split(":")[0].strip(): get_python_element_instance(one.split(":")[1].strip()) for one in values}
            a_element.general_value = values
        else:
            # unknow, treat it as string
            a_element.type = "string"
            a_element.general_value = str(a_variable_name_or_raw_value)
        return a_element
    else:
        return global_variable_dict[a_variable_name_or_raw_value]

def handle_one_line_operations(one_line_code):
    #return eval(one_line_code)
    if " + " in one_line_code:
        parts = one_line_code.split(" + ")
        parts = [get_python_element_instance(one) for one in parts]
        value = parts[0].general_value
        for one_value in parts[1:]:
            value += one_value.general_value

        an_element = Python_Element_Instance()
        an_element.type = "string"
        an_element.general_value = value

        return an_element
    elif " - " in one_line_code:
        pass
    elif " > " in one_line_code:
        pass
    elif " < " in one_line_code:
        pass
    elif " == " in one_line_code:
        pass
    else:
        return get_python_element_instance(one_line_code)

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

def process(text_code):
    # handle code, mainly just for codes inside of a function
    lines = text_code.split("\n")
    line_index = 0
    while line_index < len(lines):
        line = lines[line_index]
        if " = " in line:
            # we save that variable to global variable dict
            key, value = line.split(" = ")
            key, value = key.strip(), value.strip()
            an_element = handle_one_line_operations(value)
            an_element.name = key
            global_variable_dict[key] = an_element
        elif "print(" in line:
            key = line.split("print(")[1].split(")")[0]
            value_instance = handle_one_line_operations(key)
            general_print(value_instance)
        elif line.startswith("def "):
            # it is a function, we should save it in somewhere
            function_name = line.split("def ")[1].split("(")[0]
            function_code = ""
            end_of_a_function_new_line_counting = 0

            temp_index = line_index + 1
            while temp_index < len(lines):
                temp_line = lines[temp_index]
                if temp_line.strip() != "":
                    function_code += line + "\n"
                else:
                    end_of_a_function_new_line_counting += 1
                if end_of_a_function_new_line_counting >= 1:
                    break
                temp_index += 1

            an_element = Python_Element_Instance()
            an_element.type = "function"
            an_element.name = function_name
            an_element.general_value = function_code
            global_variable_dict[function_name] = an_element
        elif (not line.startswith("def ")) and "()" in line:
            # it is calling a function
            function_name = line.split("(")[0]
            an_element = global_variable_dict[function_name]
            if an_element.type == "function":
                process(an_element.general_value)

        line_index += 1

a_py_file_text = """
parent_variable = "parent"
print(parent_variable)

def a_function_1():
    a_child_variable = "whatever"
    print(parent_variable)
    print(a_child_variable)

a_function_1()

def a_function_2():
    temp_1 = " you say"
    a_child_variable2 = "whatever" + temp_1
    print(a_child_variable2)

a_function_2()
print("is" + " right")

a_dict = {a: 3}
print(a_dict)

a_list = ["god", "yingshaoxo"]
print(a_list)
"""

process(a_py_file_text)
