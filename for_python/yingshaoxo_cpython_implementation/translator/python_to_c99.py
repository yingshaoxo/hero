
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




translated_code = """
#include "../y_python.h"
#include "../y_python_linux.h"

"""

lines = code_text.split("\n")
i = 0
while i < len(lines):
    line = lines[i]
    # you have to implement the logic here, it is similar to what cython_to_c did, but the base python library is replaced by 'y_python.h' than 'Python.h'
    # 'y_python.h' is better because you can't easily do the static compile with 'Python.h', which makes other people hard to use your compiled c program
    i = i + 1

with open("./dist/y_python.c", "w", encoding="utf-8") as f:
    f.write(translated_code)
print(translated_code)
