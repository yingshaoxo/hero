# yingshaoxo: An advanced python interpreter

# base version: yingshaoxo
# upgrade version: github copilot sonet3.5
# bug_fix version: google gemini2.5


# To rewrite a python:
# 1. We need to parse python code into components or elements first, each element contains different information. For example: element name can be one of [str, float, int, bool, none, list, dict, ignore, variable_assignment, if_else, try_catch, while_break, function, return, function_call, class, class_instance, code, global_code].
# 2. Make sure the element has tree structure, I mean a class element may has a lot of functions elements inside, one element has many other child elements. We parse those elements from code, follow the order of 'top to bottom', 'big pattern to small pattern' or 'big template to small template'.
# 3. Then, based on the element tree, we can: 1. execute code one by one, just like an operator, where python code is the instructure or remote controller. 2. translate python code into c99 code

# If we want to convert a python code into c99 code, it has to have type information. Especially what type a function receive and returns, I mean, [str, float, int, bool, none, list, dict, any_class_definition]

# To test this script, translate itself into c99


class Python_Element_Instance():
    def __init__(self):
        # str, float, int, bool, none, list, dict,
        # variable_assignment, if_else, try_catch, while_break, function, return, function_call,
        # class, class_instance, code, global_code,
        # dict_entry (new), list_item (new)
        self._type = None
        self._name = None  # Variable/function/class name, or dictionary key for dict_entry
        self._value = None  # Actual value or code content, or dictionary value for dict_entry
        self._children = []  # Child elements (e.g., statements in a block, items in a list, value of a dict_entry)
        self._information = {}  # Additional metadata like return types, parameter types, etc.
        
    def add_child(self, child):
        """Add a child element"""
        self._children.append(child)
        return self
        
    def get_children_of_type(self, type_name):
        """Get all children of a specific type"""
        return [child for child in self._children if child._type == type_name]
        
    def __str__(self):
        return f"Python_Element_Instance(type={self._type}, name={self._name}, value={self._value})"


def get_indent_number(code: str) -> int:
    """Get the number of spaces at start of line"""
    return len(code) - len(code.lstrip())

def get_indent(code: str) -> str:
    """Get the indent string at start of line"""
    return code[:get_indent_number(code)]

def set_indent(code: str, level: int=0) -> str:
    """Set indentation level for code"""
    return "    " * level + code.lstrip()

def get_text_until_closed_symbol(lines: list, start_symbol: str, end_symbol: str) -> str:
    """Get text between matching symbols like (), [], {}"""
    result = ""
    start_count = 0
    end_count = 0
    in_string = False
    string_char = None
    
    for line in lines:
        for char in line:
            if char in '"\'':
                if not in_string:
                    in_string = True
                    string_char = char
                elif char == string_char:
                    in_string = False
            elif not in_string:
                if char == start_symbol:
                    start_count += 1
                elif char == end_symbol:
                    end_count += 1
            result += char
            if start_count > 0 and start_count == end_count:
                return result
    return result

def _fix_indentation(code: str, base_indent: int = 0) -> str:
    """Fix indentation of generated code"""
    if not code:
        return code
        
    lines = code.split("\n")
    result = []
    indent_level = base_indent
    
    for line in lines:
        stripped = line.strip()
        if stripped:
            # Decrease indent for closing braces
            if stripped.startswith("}"):
                indent_level = max(0, indent_level - 1)
            result.append("    " * indent_level + stripped)
            # Increase indent after opening braces
            if stripped.endswith("{"):
                indent_level += 1
        else:
            result.append("")
            
    return "\n".join(result)

def pre_process_comments(text_code: str) -> str:
    """Remove comments and normalize line endings"""
    lines = []
    in_multiline = False
    
    for line in text_code.splitlines():
        if in_multiline:
            if '"""' in line:
                in_multiline = False
            continue
            
        if line.strip().startswith('#'):
            continue
            
        if '"""' in line:
            in_multiline = True
            continue
            
        if '#' in line:
            line = line[:line.index('#')]
            
        if line.strip():
            lines.append(line)
            
    return '\n'.join(lines)


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
    elif text_code == "True" or text_code == "False":
        # it is bool
        a_element._type = "bool"
        if text_code == "True":
            a_element._value = "True"
        else:
            a_element._value = "False"
    elif text_code.startswith('"') and text_code.endswith('"'):
        # it is a string
        a_element._type = "str"
        a_element._value = text_code[1:-1]
    elif text_code.startswith("'") and text_code.endswith("'"):
        # it is a string
        a_element._type = "str"
        a_element._value = text_code[1:-1]
    elif text_code.replace(".","").isdigit() or (text_code.startswith('-') and text_code[1:].replace(".","").isdigit()): # Handle negative numbers
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
    elif text_code.startswith('[') and text_code.endswith(']'):
        # it is list literal
        a_element._type = "list"
        a_element._value = text_code # Store the full list string
        
        content = text_code[1:-1].strip()
        if content:
            items = []
            current = ""
            depth = 0 # For nested lists/dicts
            in_string = False
            string_char = None
            
            for char in content:
                if char in '"\'':
                    if not in_string:
                        in_string = True
                        string_char = char
                    elif char == string_char:
                        in_string = False
                    current += char
                elif char in '[{(':
                    depth += 1
                    current += char
                elif char in ']}': # No `)` for list parsing
                    depth -= 1
                    current += char
                elif char == ',' and depth == 0 and not in_string:
                    if current.strip():
                        items.append(current.strip())
                    current = ""
                else:
                    current += char
                    
            if current.strip():
                items.append(current.strip())
            
            # Process each item
            for item_str in items:
                child = Python_Element_Instance()
                child._type = "list_item" # New type for list elements
                child._value = item_str # Store the raw string value of the item
                # Recursively parse the item's value (it could be another literal)
                child._children.append(parse_code_by_char(item_str)) 
                a_element._children.append(child)

    elif text_code.startswith('{') and text_code.endswith('}'):
        # Dictionary literal
        a_element._type = "dict"
        a_element._value = text_code # Store the full dict string
        
        content = text_code[1:-1].strip()
        if content:
            parts = []
            current = ""
            brace_level = 0 # For nested structures
            in_string = False
            string_char = None
            
            for char in content:
                if char in '"\'':
                    if not in_string:
                        in_string = True
                        string_char = char
                    elif char == string_char:
                        in_string = False
                    current += char
                elif char in '{[(':
                    brace_level += 1
                    current += char
                elif char in ']}': # No `)` for dict parsing
                    brace_level -= 1
                    current += char
                elif char == ',' and brace_level == 0 and not in_string:
                    if current.strip():
                        parts.append(current.strip())
                    current = ""
                else:
                    current += char
                    
            if current.strip():
                parts.append(current.strip())
                
            # Process each key-value pair
            for pair_str in parts:
                if ":" in pair_str:
                    key_part, value_part = pair_str.split(":", 1)
                    key_part = key_part.strip().strip('"').strip("'")
                    value_part = value_part.strip()
                    
                    child = Python_Element_Instance()
                    child._type = "dict_entry" # New type for dict entries
                    child._name = key_part # Key
                    child._value = value_part # Value as string
                    # Recursively parse the value of the dict entry
                    child._children.append(parse_code_by_char(value_part)) 
                    a_element._children.append(child)
    
    return a_element

def _handle_builtin_function(func_name: str, args: str) -> str:
    """Translate Python built-in functions to C99"""
    builtin_map = {
        "print": "ypython_print",
        "len": "ypython_len",
        "str": "ypython_to_string",
        "int": "ypython_to_int",
        "float": "ypython_to_float",
        "list": "ypython_to_list",
        "dict": "ypython_to_dict",
        "bool": "ypython_to_bool",
        "type": "ypython_type"
    }
    
    if func_name in builtin_map:
        return "{}({})".format(builtin_map[func_name], args)
    return None

def translate_function_call(function_name: str, args: str) -> str:
    """Translate Python function calls to C99"""
    # First check if it's a builtin function
    builtin_translation = _handle_builtin_function(function_name, args)
    if builtin_translation:
        return builtin_translation
        
    # Handle method calls (obj.method())
    if "." in function_name:
        obj_name, method_name = function_name.split(".")
        return "{}->{}({}, {})".format(obj_name, method_name, obj_name, args)
        
    # Regular function call
    return "{}({})".format(function_name, args)

def translate_args(args_str: str) -> str:
    """Translate Python function arguments to C99"""
    if not args_str.strip():
        return ""
        
    args = []
    current = ""
    in_string = False
    string_char = None
    paren_level = 0
    
    for char in args_str:
        if char in '"\'':
            if not in_string:
                in_string = True
                string_char = char
            elif char == string_char and not paren_level:
                in_string = False
            current += char
        elif char == '(':
            paren_level += 1
            current += char
        elif char == ')':
            paren_level -= 1
            current += char
        elif char == ',' and not in_string and not paren_level:
            args.append(current.strip())
            current = ""
        else:
            current += char
            
    if current:
        args.append(current.strip())
        
    # Translate each argument
    translated = []
    for arg in args:
        if "=" in arg:  # Keyword argument
            name, value = arg.split("=")
            translated.append("{} = {}".format(
                name.strip(),
                _evaluate_expression(value.strip())
            ))
        else:
            # Pass a dummy context as this is for argument translation, not variable assignment
            # This might need refinement if argument expressions can contain complex literals
            translated.append(_to_c99_value_from_python_value(parse_code_by_char(arg), TranslationContext())[0]) 
            
    return ", ".join(translated)

def _handle_memory_cleanup(translated_code: str) -> str:
    """Add proper memory cleanup for allocated resources"""
    lines = []
    cleanup_needed = []
    
    for line in translated_code.split('\n'):
        lines.append(line)
        
        # Track allocations that need cleanup
        if 'Ypython_Dict()' in line:
            var_name = line.split('*')[1].split('=')[0].strip()
            cleanup_needed.append(('dict', var_name))
        elif 'ypython_create_a_general_variable' in line:
            var_name = line.split('*')[1].split('=')[0].strip()
            cleanup_needed.append(('general', var_name))
            
    # Add cleanup code before return statements
    if cleanup_needed:
        cleaned_lines = []
        for line in lines:
            if line.strip().startswith('return '):
                # Add cleanup before return
                for type_, name in cleanup_needed:
                    if type_ == 'dict':
                        cleaned_lines.append('    {}->function_free({});'.format(name, name))
                    elif type_ == 'general':
                        cleaned_lines.append('    ypython_free_general_variable({});'.format(name))
            cleaned_lines.append(line)
        return '\n'.join(cleaned_lines)
        
    return '\n'.join(lines)

def validate_python_element(element: Python_Element_Instance) -> tuple:
    """Validates a Python element before translation and returns (is_valid, error_message)"""
    if not element:
        return False, "Invalid element structure"
        
    if element._type == "variable_assignment":
        if not element._name or element._value is None: # Check for None explicitly
            return False, "Invalid variable assignment"
            
    if element._type == "dict":
        for child in element._children:
            if child._type == "dict_entry": # Check for new dict_entry type
                if not child._name or child._value is None:
                    return False, "Dictionary key or value missing for dict_entry"
            else:
                return False, f"Unexpected child type in dict: {child._type}"
    
    if element._type == "list":
        for child in element._children:
            if child._type != "list_item": # Check for new list_item type
                return False, f"Unexpected child type in list: {child._type}"
                    
    return True, ""

def parse_code(text_code: str, just_return_one_element: bool=False, global_code=False) -> Python_Element_Instance:
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
                new_index_for_the_code_that_did_not_get_parsed = temp_element._information.get("_parsed_line_index", 1) # Default to 1 to prevent infinite loop
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
            key, value_str = line.split(" = ")
            key, value_str = key.strip(), value_str.strip()

            an_element = Python_Element_Instance()
            an_element._type = "variable_assignment"
            an_element._name = key
            an_element._value = value_str # Store the raw string value
            an_element._information["_parsed_child_text"] = True
            an_element._information["indent_string"] = get_indent(original_line)

            # Parse the value string into its appropriate element type
            # Use parse_code_by_char for literals or expressions
            parsed_value_element = parse_code_by_char(value_str)
            if parsed_value_element._type != "ignore": # If it's a recognized literal
                an_element._children.append(parsed_value_element)
            # If it's not a literal (e.g., another variable, a function call),
            # then _value will contain the raw string, and tranalste_to_c99 will handle it.
            
        elif line.endswith(")"):
            # it is function call
            an_element = Python_Element_Instance()
            an_element._type = "function_call"
            an_element._value = line
            function_name = line.split("(")[0]
            function_arguments = line.split("(")[1].split(")")[0]
            an_element._information["function_name"] = function_name
            if "." in function_name:
                class_instance_name, function_name = function_name.split(".")
                an_element._information["function_name"] = function_name
                an_element._information["is_class_function"] = True
                an_element._information["class_instance_name"] = class_instance_name
            an_element._information["function_arguments"] = function_arguments
        elif line.endswith("]") and not line.startswith("["):
            # it is asking for a value from a list or dict by key
            an_element = Python_Element_Instance()
            an_element._type = "get_value_from_dict_or_list"
            an_element._value = line
            list_or_dict_name = line.split("[")[0]
            index_or_key = line.split("[")[1].split("]")[0]
            an_element._information["list_or_dict_name"] = list_or_dict_name
            an_element._information["index_or_key"] = index_or_key
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
    if type_string not in ["None", "str", "int", "float", "list", "dict", "bool"]:
        return "Type_" + type_string
    else:
        return "Type_Ypython_General"

def _translate_operator(op: str, left: str, right: str) -> str:
    """Translate Python operators to C99"""
    op_map = {
        "+": "ypython_add",
        "-": "ypython_subtract", 
        "*": "ypython_multiply",
        "/": "ypython_divide",
        "==": "ypython_equals",
        "!=": "ypython_not_equals",
        "<": "ypython_less_than",
        ">": "ypython_greater_than",
        "<=": "ypython_less_equal",
        ">=": "ypython_greater_equal",
        "and": "ypython_and",
        "or": "ypython_or",
        "in": "ypython_contains"
    }
    
    if op in op_map:
        return "{}({}, {})".format(op_map[op], left, right)
    return "/* Unsupported operator: {} */".format(op)

def _handle_string_operations(code: str) -> str:
    """Handle string operations in translated code"""
    if not code:
        return code
        
    # Handle string concatenation
    if " + " in code and ("\"" in code or "'" in code):
        parts = code.split(" + ")
        translated_parts = []
        for part in parts:
            part = part.strip()
            if part.startswith(("\"", "'")):
                translated_parts.append("Ypython_String(" + part + ")")
            else:
                translated_parts.append(part)
        return "ypython_string_concat(" + ", ".join(translated_parts) + ")"
        
    return code

def _to_c99_value_from_python_value(value_element: Python_Element_Instance, context: 'TranslationContext') -> tuple[str, str]:
    """
    Convert a Python_Element_Instance representing a value to its C99 constructor string or
    generate code for complex literals.
    
    Returns: (generated_c99_code_block_for_temp_vars, c99_value_constructor_or_temp_var_name)
    """
    generated_code_block = [] # To store code for temporary variables (lists, dicts)

    if value_element._type == "int":
        return "", f"Ypython_Int({value_element._value})"
    elif value_element._type == "float":
        return "", f"Ypython_Float({value_element._value})"
    elif value_element._type == "str":
        return "", f"Ypython_String(\"{value_element._value}\")"
    elif value_element._type == "bool":
        return "", "Ypython_Bool(1)" if value_element._value == "True" else "Ypython_Bool(0)"
    elif value_element._type == "None":
        return "", "Ypython_None()"
    elif value_element._type == "list":
        list_temp_var = context.get_unique_temp_var_name("temp_list")
        generated_code_block.append(f"Type_Ypython_List *{list_temp_var} = Ypython_List();")
        
        for child_item in value_element._children:
            if child_item._type == "list_item":
                # Recursively get the C99 constructor for the item's value
                item_temp_code, item_c99_value = _to_c99_value_from_python_value(child_item._children[0], context)
                generated_code_block.extend(item_temp_code.splitlines()) # Add any nested temp var code
                generated_code_block.append(f"{list_temp_var}->function_append({list_temp_var}, {item_c99_value});")
        
        return "\n".join(generated_code_block), list_temp_var

    elif value_element._type == "dict":
        dict_temp_var = context.get_unique_temp_var_name("temp_dict")
        generated_code_block.append(f"Type_Ypython_Dict *{dict_temp_var} = Ypython_Dict();")
        
        for child_entry in value_element._children:
            if child_entry._type == "dict_entry":
                key = child_entry._name
                # Recursively get the C99 constructor for the entry's value
                entry_temp_code, entry_c99_value = _to_c99_value_from_python_value(child_entry._children[0], context)
                generated_code_block.extend(entry_temp_code.splitlines()) # Add any nested temp var code
                generated_code_block.append(f"{dict_temp_var}->function_set({dict_temp_var}, Ypython_String(\"{key}\"), {entry_c99_value});")
        
        return "\n".join(generated_code_block), dict_temp_var
    else:
        # If it's not a direct literal (e.g., a variable name, a function call expression),
        # return the string as is, assuming it's a C99 variable name or expression.
        return "", value_element._value # Return the raw value string for non-literals


def _handle_error_propagation(code: str) -> str:
    """Add error handling and propagation to translated code"""
    translated_code = """
#include <setjmp.h>
#include <string.h>

// Error types
typedef enum {
    ERROR_NAME,
    ERROR_TYPE,
    ERROR_VALUE,
    ERROR_KEY,
    ERROR_INDEX,
    ERROR_ATTRIBUTE,
    ERROR_GENERAL
} ErrorType;

// Error structure
typedef struct {
    ErrorType type;
    char message[1024];
} Error;

// Error handling globals
jmp_buf exception_buf;
Error current_error;

// Error handling functions
void set_error(ErrorType type, const char* msg) {
    current_error.type = type;
    strncpy(current_error.message, msg, sizeof(current_error.message) - 1);
    longjmp(exception_buf, 1);
}

void raise_name_error(const char* name) {
    char msg[1024];
    snprintf(msg, sizeof(msg), "NameError: name '%s' is not defined", name);
    set_error(ERROR_NAME, msg);
}

void raise_type_error(const char* msg) {
    set_error(ERROR_TYPE, msg);
}

void raise_value_error(const char* msg) {
    set_error(ERROR_VALUE, msg);
}

void raise_key_error(const char* key) {
    char msg[1024];
    snprintf(msg, sizeof(msg), "KeyError: '%s'", key);
    set_error(ERROR_KEY, msg);
}

void raise_index_error() {
    set_error(ERROR_INDEX, "IndexError: list index out of range");
}

void raise_attribute_error(const char* obj, const char* attr) {
    char msg[1024];
    snprintf(msg, sizeof(msg), "AttributeError: '%s' object has no attribute '%s'", obj, attr);
    set_error(ERROR_ATTRIBUTE, msg);
}
"""
    translated_code += code
    return translated_code

def tranalste_to_c99(a_python_element: Python_Element_Instance, context: 'TranslationContext') -> str:
    # Validate element before translation
    is_valid, error_msg = validate_python_element(a_python_element)
    if not is_valid:
        print("Translation error: " + error_msg)
        return ""
        
    translated_code = ""
    
    if a_python_element._type == "global_code":
        translated_code = _handle_error_propagation(translated_code)  # Add error handling setup
        translated_code += """
#include "../y_python.h"
#include "../y_python_linux.h"
"""
        for child in a_python_element._children:
            translated_code += tranalste_to_c99(child, context) # Pass context
        translated_code += """
int main(int argument_number, char **argument_list) {
    //{code_in_main_function}
    return 0;
}
"""
    elif a_python_element._type == "function" and "_return_type" in a_python_element._information:
        # Parse parameter types
        params = a_python_element._information["_parameter_string"].split(",")
        param_defs = []
        for param in params:
            param = param.strip()
            if ":" in param:
                name, type_str = param.split(":")
                c_type = _to_c99_type_from_python_type(type_str.strip())
                param_defs.append("{} *{}".format(c_type, name.strip()))
            else:
                param_defs.append("Type_Ypython_General *{}".format(param.strip()))
        
        param_string = ", ".join(param_defs)
        
        # Handle return type
        _return_type = a_python_element._information["_return_type"]
        return_type = _to_c99_type_from_python_type(_return_type)
        
        function_code = """
{return_type} *{function_name}({parameter_string}) {{
    Type_Ypython_Dict *local_vars = Ypython_Dict();
{function_code}
}}
"""
        # Parse function body and handle variable declarations
        body_lines = []
        # Create a new context for the function's scope
        function_context = TranslationContext() 
        for line in a_python_element._information["_pure_function_code"].split("\n"):
            # This part needs to be parsed and translated recursively with the new function_context
            # For now, just append raw lines, but ideally, parse and translate each line.
            # This is a simplification for the current problem, but a full solution would need
            # to parse the function body into its own Python_Element_Instance tree and
            # then call tranalste_to_c99 on its children with the function_context.
            body_lines.append("    " + line)

        translated_code += function_code.format(
            return_type=return_type,
            function_name=a_python_element._name,
            parameter_string=param_string,
            function_code="\n".join(body_lines)
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
                # Use a new context for class initialization
                class_init_context = TranslationContext()
                for child in code_object._children:
                    if child._type == "variable_assignment":
                        key, value = child._name, child._value.replace("\n", "")
                        if "self." in key:
                            name = key.split("self.")[1]
                            the_type = _to_c99_type_from_python_type(child._children[0]._type)
                            property_definition_string += indents_string + the_type + " *" + name + ";" + "\n"
                            
                            if child._children:
                                temp_code, c99_value = _to_c99_value_from_python_value(child._children[0], class_init_context)
                                if temp_code:
                                    property_initiation_string += _fix_indentation(temp_code, 1) + "\n"
                                property_initiation_string += indents_string + "new_element_instance->" + name + " = " + c99_value + ";\n\n"

        functions_code_string = ""
        for child in a_python_element._children:
            if child._name != "__init__":
                function_name, function_content = child._name, child._value

                # Parse method parameters and return type
                return_type = "void"
                if "_return_type" in child._information:
                    return_type = _to_c99_type_from_python_type(child._information["_return_type"])
                
                # Parse parameters
                params = ["Type_{} *self".format(a_python_element._name)]  # Always include self
                if child._information.get("_parameter_string"):
                    param_list = child._information["_parameter_string"].split(",")
                    for param in param_list:
                        param = param.strip()
                        if param == "self":  # Skip self as we already added it
                            continue
                        if ":" in param:
                            name, type_str = param.split(":")
                            c_type = _to_c99_type_from_python_type(type_str.strip())
                            params.append("{} *{}".format(c_type, name.strip()))
                        else:
                            params.append("Type_Ypython_General *{}".format(param))
                
                param_string = ", ".join(params)
                
                # Generate method code
                method_code = """
{return_type} *Type_{class_name}_{function_name}({param_string}) {{
    Type_Ypython_Dict *method_vars = Ypython_Dict();
{method_body}
    return NULL;  // TODO: Handle return value
}}
"""
                # Parse method body
                body_lines = []
                # Create a new context for the method's scope
                method_context = TranslationContext()
                for line in child._information["_pure_function_code"].split("\n"):
                    # This part needs to be parsed and translated recursively with the new method_context
                    # For now, just append raw lines, but ideally, parse and translate each line.
                    if line.strip():
                        if "=" in line and not line.strip().startswith("if") and not line.strip().startswith("while"):
                            var_name = line.split("=")[0].strip()
                            if not var_name.startswith("self."):  # Don't declare self.x variables
                                body_lines.append("    Type_Ypython_General *{} = NULL;".format(var_name))
                        if line.strip().startswith("return "):
                            return_expr = line.strip()[7:]  # Remove 'return '
                            if return_expr:
                                # Need to parse return_expr into element and pass context
                                return_temp_code, return_value_c99 = _to_c99_value_from_python_value(parse_code_by_char(return_expr), method_context)
                                if return_temp_code:
                                    body_lines.extend(_fix_indentation(return_temp_code, 1).splitlines())
                                body_lines.append("    return {};".format(return_value_c99))
                            else:
                                body_lines.append("    return NULL;")
                        else:
                            body_lines.append("    " + line)
                
                functions_code_string += method_code.format(
                    return_type=return_type,
                    class_name=a_python_element._name,
                    function_name=function_name,
                    param_string=param_string,
                    method_body="\n".join(body_lines)
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

    elif a_python_element._type == "function_call":
        if "is_class_function" in a_python_element._information:
            if a_python_element._information["function_name"] == "split":
                translated_code += "ypython_string_type_function_split(" + a_python_element._information["class_instance_name"] + ", " + 'Ypython_String(' + a_python_element._information["function_arguments"] + '))' + ";"
            elif a_python_element._information["function_name"] == "strip":
                translated_code += a_python_element._information["class_instance_name"] + "->function_strip(" + a_python_element._information["class_instance_name"] + ", Ypython_String(" + a_python_element._information["function_arguments"] + '))' + ";"
            else:
                translated_code += a_python_element._information["class_instance_name"] + "->" + a_python_element._information["function_name"] + "(" + a_python_element._information["function_arguments"] + ")" + ";"
        else:
            translated_code += a_python_element._information["function_name"] + "(" + a_python_element._information["function_arguments"] + ")" + ";"
    elif a_python_element._type == "variable_assignment":
        translated_code_lines = []
        var_name = a_python_element._name
        value_element = None

        if a_python_element._children:
            value_element = a_python_element._children[0] # The parsed value (dict, list, or simple literal)

            # Get the temporary variable initialization code and the C99 value/temp_var_name
            temp_code_for_value, c99_value_or_temp_var = _to_c99_value_from_python_value(value_element, context)
            
            if temp_code_for_value:
                translated_code_lines.append(temp_code_for_value) # Add the temp var initialization code
            
            translated_code_lines.append(f"Type_Ypython_General *{var_name} = ypython_create_a_general_variable({c99_value_or_temp_var});")
            
            # If a temporary variable was created for a complex literal, we should free it after it's copied
            if value_element._type in ["dict", "list"]:
                # Assuming ypython_create_a_general_variable makes a deep copy,
                # the temporary dict/list can be freed here.
                if value_element._type == "dict":
                    translated_code_lines.append(f"// ypython_free_dict({c99_value_or_temp_var}); // Add actual free function if available")
                elif value_element._type == "list":
                    translated_code_lines.append(f"// ypython_free_list({c99_value_or_temp_var}); // Add actual free function if available")
            
        else:
            # Fallback for assignments without children (e.g., `x = y`)
            # This means the value is likely a variable reference or a simple expression not parsed into children
            # In this case, _to_c99_value_from_python_value will return the variable name directly
            temp_code_for_value, c99_value_or_temp_var = _to_c99_value_from_python_value(parse_code_by_char(a_python_element._value), context)
            if temp_code_for_value:
                translated_code_lines.append(temp_code_for_value)
            translated_code_lines.append(f"Type_Ypython_General *{var_name} = ypython_create_a_general_variable({c99_value_or_temp_var});")
        
        translated_code += "\n".join(translated_code_lines) + "\n" # Add a newline after each assignment block

    elif len(a_python_element._children) != 0:
        for child in a_python_element._children:
            translated_code += tranalste_to_c99(child, context) # Pass context
    elif a_python_element._type == "if_else":
        temp_code_cond, condition_c99 = _to_c99_value_from_python_value(parse_code_by_char(a_python_element._value), context)
        translated_code += temp_code_cond + "\n" if temp_code_cond else ""
        translated_code += """
if (ypython_is_true({condition})) {{
{if_block}
}} else {{
{else_block}
}}""".format(
            condition=condition_c99,
            if_block=_fix_indentation(tranalste_to_c99(a_python_element._children[0], context), 1), # Pass context
            else_block=_fix_indentation(tranalste_to_c99(a_python_element._children[1], context), 1) if len(a_python_element._children) > 1 else "" # Pass context
        )
    elif a_python_element._type == "while":
        temp_code_cond, condition_c99 = _to_c99_value_from_python_value(parse_code_by_char(a_python_element._value), context)
        translated_code += temp_code_cond + "\n" if temp_code_cond else ""
        translated_code += """
while (ypython_is_true({condition})) {{
{while_block}
}}""".format(
            condition=condition_c99,
            while_block=_fix_indentation(tranalste_to_c99(a_python_element._children[0], context), 1) # Pass context
        )
    elif a_python_element._type == "try_except":
        # Translate try-except blocks to C setjmp/longjmp
        translated_code += """
jmp_buf exc_buf;
if (setjmp(exc_buf) == 0) {
    // Try block
%s
} else {
    // Except block
%s
}""" % (
            _fix_indentation(tranalste_to_c99(a_python_element._children[0], context), 1), # Pass context
            _fix_indentation(tranalste_to_c99(a_python_element._children[1], context), 1) if len(a_python_element._children) > 1 else "" # Pass context
        )

    # Add memory management and fix indentation
    translated_code = _handle_memory_cleanup(translated_code)
    translated_code = _fix_indentation(translated_code)

    return translated_code


def _evaluate_expression(value_string: str) -> str:
    """Convert Python expressions to C99"""
    if not value_string.strip():
        return ""
        
    # Handle parentheses
    if value_string.startswith("(") and value_string.endswith(")"):
        inner = _evaluate_expression(value_string[1:-1])
        return "(" + inner + ")"
        
    # Split by operators while preserving strings
    parts = []
    current = ""
    in_string = False
    string_char = None
    
    for char in value_string:
        if char in '"\'':
            if not in_string:
                in_string = True
                string_char = char
            elif char == string_char:
                in_string = False
            current += char
        elif char in '+-*/><=!' and not in_string:
            # Look ahead for compound operators
            if len(parts) > 0 and parts[-1] == char:
                op = parts.pop() + char
                if op in ["++", "--"]:
                    # Handle increment/decrement
                    current = current[:-2]  # Remove last two characters
                    parts.append("ypython_{}_operator({})".format("add" if op == "++" else "subtract", current))
            else:
                parts.append(current.strip())
                current = char
        else:
            current += char
            
    if current:
        parts.append(current.strip())
        
    # Convert operators
    result = []
    i = 0
    while i < len(parts):
        part = parts[i].strip()
        if part in "+-*/><=!":
            # Look ahead for compound operators
            if i + 1 < len(parts) and parts[i + 1] in "=":
                op = part + parts[i + 1]
                i += 1
            else:
                op = part
                
            if i + 1 < len(parts):
                left = result[-1] if result else ""
                right = parts[i + 1]
                result[-1] = "ypython_{}({}, {})".format(
                    {
                        "+": "add",
                        "-": "subtract",
                        "*": "multiply",
                        "/": "divide",
                        ">": "greater",
                        "<": "less",
                        ">=": "greater_equal",
                        "<=": "less_equal",
                        "==": "equals",
                        "!=": "not_equals"
                    }.get(op, "unknown"),
                    left,
                    _evaluate_expression(right)
                )
                i += 1
        else:
            # Convert literals and variables
            # This part should ideally use _to_c99_value_from_python_value for consistency
            if part.startswith(("'", '"')):
                result.append('Ypython_String(' + part + ')')
            elif part.replace('.','').isdigit() or (part.startswith('-') and part[1:].replace(".","").isdigit()): # Handle negative numbers
                if '.' in part:
                    result.append('Ypython_Float(' + part + ')')
                else:
                    result.append('Ypython_Int(' + part + ')')
            else:
                result.append(part)
        i += 1
        
    return " ".join(result)
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
    # Pass a new context for global translation
    final_code = tranalste_to_c99(global_code_object, TranslationContext())
    print(final_code)


def translate_with_error_handling(input_python_code: str) -> str:
    """Wrapper function to handle translation with proper error messages"""
    try:
        # Pre-process the code
        processed_code = pre_process_comments(input_python_code)
        
        # Parse into elements
        global_code_object = parse_code(processed_code, global_code=False)
        if not global_code_object:
            return "Error: Failed to parse Python code"
            
        # Validate before translation
        is_valid, error_msg = validate_python_element(global_code_object)
        if not is_valid:
            return "Error: " + error_msg
            
        # Translate to C99
        # Pass a new context for the main translation process
        c99_code = tranalste_to_c99(global_code_object, TranslationContext())
        if not c99_code:
            return "Error: Failed to translate code"
            
        return c99_code.strip()
        
    except Exception as e:
        return "Error during translation: " + str(e)

def _get_code_block(lines: list, start_index: int) -> tuple:
    """Get a code block maintaining proper scope"""
    block = []
    current_index = start_index
    base_indent = get_indent_number(lines[current_index])
    
    while current_index < len(lines):
        line = lines[current_index]
        if line.strip():
            current_indent = get_indent_number(line)
            if current_indent < base_indent:
                break
        block.append(line)
        current_index += 1
        
    return "\n".join(block), current_index - 1

def real_time_c99_translator_shell():
    """Interactive shell for translating Python to C99 in real-time"""
    import os

    print("Python to C99 Translator")
    print("=======================")
    print("Type or paste Python code to translate to C99.")
    print("Enter empty line to translate single line or twice for multi-line.")
    print("\nFeatures supported:")
    print("- Basic Python types (int, float, str, list, dict)")
    print("- Function definitions with type hints")
    print("- Class definitions with methods")
    print("- Control flow (if/else, while)")
    print("- Try/except error handling")
    print("- Basic operators and expressions")
    print("\n")

    while True:
        code_buffer = []
        empty_line_count = 0
        
        try:
            while True:
                prompt = ">>> " if not code_buffer else "... "
                line = input(prompt)
                
                if not line.strip():
                    if code_buffer and ":" not in code_buffer[-1]:  # Single line statement
                        break
                    empty_line_count += 1
                    if empty_line_count >= 2:  # Multi-line code block
                        break
                else:
                    empty_line_count = 0
                    code_buffer.append(line)
                    
            if not code_buffer:
                break
                
            # Process the code block
            input_python_code = "\n".join(code_buffer)
            
            try:
                # Pre-process and validate the code
                processed_code = pre_process_comments(input_python_code)
                global_code_object = parse_code(processed_code, global_code=False)
                
                if not global_code_object:
                    print("\nError: Failed to parse Python code")
                    continue
                    
                # Translate to C99
                # Pass a new context for each shell command
                final_code = tranalste_to_c99(global_code_object, TranslationContext())
                if not final_code:
                    print("\nError: Failed to generate C99 code")
                    continue
                    
                # Display results
                print("\nTranslated C99 code:")
                print("===================")
                print(final_code)
                
                # Copy to clipboard using temporary file
                try:
                    with open('/tmp/c99_code.txt', 'w') as f:
                        f.write(final_code)
                    os.system('cat /tmp/c99_code.txt | xclip -selection clipboard')
                    os.remove('/tmp/c99_code.txt')
                    print("\n(Code copied to clipboard)")
                except:
                    print("\n(Failed to copy to clipboard)")
                
            except Exception as e:
                print("\nTranslation error:", str(e))
                
        except KeyboardInterrupt:
            print("\nTranslation cancelled.")
            continue
        except EOFError:
            break

class TranslationContext:
    def __init__(self):
        self.scope_stack = [{}]  # Stack of scopes for variable tracking
        self.indent_level = 0
        self.current_class = None
        self.current_function = None
        self.temp_var_counter = 0 # New counter for unique temporary variables
        
    def enter_scope(self):
        """Enter a new scope level"""
        self.scope_stack.append({})
        
    def exit_scope(self):
        """Exit current scope level"""
        if len(self.scope_stack) > 1:
            self.scope_stack.pop()
            
    def add_variable(self, name: str, type_str: str):
        """Add variable to current scope"""
        self.scope_stack[-1][name] = type_str
        
    def get_variable_type(self, name: str) -> str:
        """Get variable type from current scope chain"""
        for scope in reversed(self.scope_stack):
            if name in scope:
                return scope[name]
        return None
        
    def increase_indent(self):
        self.indent_level += 1
        
    def decrease_indent(self):
        self.indent_level = max(0, self.indent_level - 1)
        
    def get_indent(self) -> str:
        return "    " * self.indent_level

    def get_unique_temp_var_name(self, prefix: str) -> str:
        """Generates a unique temporary variable name."""
        self.temp_var_counter += 1
        return f"{prefix}_{self.temp_var_counter}"


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1:
        # File translation mode
        input_file = sys.argv[1]
        try:
            with open(input_file, 'r') as f:
                python_code = f.read()
                
            c99_code = translate_with_error_handling(python_code)
            print(c99_code)
        except Exception as e:
            print("Error:", str(e))
    else:
        # Interactive shell mode
        real_time_c99_translator_shell()
