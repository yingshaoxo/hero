"""
I'm actually try to make a minimum python interpreter. As you can see, I can handle variable assignment and print it out.

But I did not implement function related code in y_python.c, can you modifying 'y_python.c' according to 'y_python.py'?
"""

# A demo python parser, definately have bugs, it is just fake code, rewrite it carefully to prevent bugs

global_variable_dict = {
    "__functions__": {}
}

def handle_one_line_operations(one_line_code):
    return eval(one_line_code)

def process(text_code):
    # handle code, mainly just for codes inside of a function
    lines = text_code.split("\n")
    target_line = 0
    for line_index in range(len(lines)):
        if line_index < target_line:
            continue
        line = lines[line_index]
        if " = " in line:
            # we save that variable to global variable dict
            key,value = line.split(" = ")
            key, value = key.strip(), value.strip()
            global_variable_dict[key] = handle_one_line_operations(value)
        elif "print(" in line:
            key = line.split("print(")[1].split(")")[0]
            if key not in global_variable_dict:
                print(key)
            else:
                print(global_variable_dict[key])
        elif line.startswith("def "):
            # it is a function, we should save it in somewhere
            function_name = line.split("def ")[1].split("(")[0]
            function_code = ""
            end_of_a_function_new_line_counting = 0
            for i in range(len(lines) - line_index):
                line_index += 1
                line = lines[line_index]
                if line.strip() != "":
                    function_code += line + "\n"
                else:
                    end_of_a_function_new_line_counting += 1
                if end_of_a_function_new_line_counting >= 1:
                    break
            target_line = line_index
            global_variable_dict["__functions__"][function_name] = function_code
        elif (not line.startswith("def ")) and "()" in line:
            # it is calling a function
            function_name = line.split("(")[0]
            function_code = global_variable_dict["__functions__"][function_name]
            process(function_code)

a_py_file_text = """
parent_variable = "parent"
print(parent_variable)

def a_function():
    a_child_variable = "whatever"
    print(parent_variable)
    print(a_child_variable)

a_function()
"""

process(a_py_file_text)
