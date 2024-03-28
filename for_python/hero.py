import sys
import os
import codecs

#print(sys.argv)

def count_prefix_space(input_text):
    counting = 0
    for char in input_text:
        if char == ' ':
            counting += 1
        else:
            break
    return counting

def convert_hero_lang_into_python(input_text):
    new_text = ""
    for line in input_text.split("\n"):
        prefix_spaces = count_prefix_space(line) * ' '
        line = line.strip()

        if line.startswith("if (") and line.endswith("{"):
            equation = line[4:-3]
            line = "if ({}):".format(equation)
        elif line == "}":
            line = ""
        elif line.startswith("define "):
            line = "def " + line[7:-2] + ":"
        else:
            line = line

        new_text += prefix_spaces + line
        new_text += "\n"
    return new_text

if (len(sys.argv) <= 1):
    print("You should use this hero python compiler as: \n'python hero.py input_file.hero output_file.py'")
else:
    input_file_path = sys.argv[1]
    if not os.path.exists(input_file_path):
        raise Exception("'{}' is not exists.".format(input_file_path))

    if len(sys.argv) <= 2:
        output_file_path = input_file_path + ".sh"
    else:
        output_file_path = sys.argv[2]

    with codecs.open(input_file_path, 'r', encoding="utf-8") as f:
        input_hero_lang_text = f.read()

    output_python_script = convert_hero_lang_into_python(input_hero_lang_text)
    with codecs.open(output_file_path, 'w', encoding="utf-8") as f:
        f.write(output_python_script)
