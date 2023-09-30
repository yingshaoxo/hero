import re
from typing import Any

from auto_everything.disk import Disk #type: ignore
from auto_everything.io import IO #type: ignore
from auto_everything.terminal import Terminal #type: ignore
disk = Disk()
io_ = IO()
terminal = Terminal()


def replace_hextag_to_double_slash(old_text: str) -> str:
    def do_a_replace(match_obj: Any):
        if match_obj.group() is not None:
            original_line = match_obj.group()
            original_line = original_line.replace("#", "//", 1)
            return original_line

    result = re.sub(r"[ \t]*#(?:.*)", do_a_replace, old_text) #type: ignore
    return result #type: ignore


def replace_three_quotation_mark_to_three_slash(old_text: str) -> str:
    def do_a_replace_1(match_obj: Any):
        if match_obj.group() is not None:
            original_line = match_obj.group()
            original_line = original_line.replace('"""', "/*", 1)
            original_line = original_line.replace('"""', "*/", 1)
            return original_line
    result = re.sub(r"([ \t]*\"\"\"(?:\s.*?)*\"\"\")", do_a_replace_1, old_text) #type: ignore

    def do_a_replace_2(match_obj: Any):
        if match_obj.group() is not None:
            original_line = match_obj.group()
            original_line = original_line.replace("'''", "/*", 1)
            original_line = original_line.replace("'''", "*/", 1)
            return original_line
    result = re.sub(r"([ \t]*\'\'\'(?:\s.*?)*\'\'\')", do_a_replace_2, result) #type: ignore

    return result #type: ignore


def replace_three_quotation_mark_comments_to_empty_string(old_text: str) -> str:
    result = re.sub(r"([ \t]*\"\"\"(?:\s.*?)*\"\"\")", "", old_text) #type: ignore
    result = re.sub(r"([ \t]*\'\'\'(?:\s.*?)*\'\'\')", "", result) #type: ignore
    return result #type: ignore


def replace_import_statement_to_real_code_block(base_dir: str, old_text: str) -> str:
    def try_to_do_a_replace(match_obj: Any):
        if match_obj.group() is not None:
            # whole_import_statement = match_obj.group()
            module_name = match_obj.group('name')
            module_nickname = match_obj.group('nickname')
            print(module_name, module_nickname)

            module_path = module_name
            if module_name.startswith("./") or module_name.startswith("/"):
                # it is a file path
                if module_name.startswith("/"):
                    # it is an absolute path, ignore it
                    pass
                else:
                    module_path = disk.join_paths(base_dir, module_path)
            else:
                # you need to find the real path from 'hero_modules' folder
                pass

            if module_nickname != None:
                module_name = module_nickname
            else:
                module_name, _ = disk.get_stem_and_suffix_of_a_file(module_name)
            
            hero_to_cpp_compiler = HeroToCppCompiler()
            file = disk.get_absolute_path(path=module_path)
            directory_path = disk.get_directory_path(path=file)
            output_folder = disk.get_a_temp_folder_path()
            disk.create_a_folder(output_folder)
            output_file_path = hero_to_cpp_compiler.compile_to_cpp_file(input_base_folder=directory_path, input_file=file, output_folder=output_folder)
            output_code = io_.read(output_file_path)
            output_code = "\n".join(["    " + line for line in output_code.split("\n")])

            code_template = f"""
namespace {module_name}
{{
{output_code}
}}
            """.strip()

            return code_template

    result = re.sub(r"^import \"(?P<name>.*)\"(?: as (?P<nickname>.*))*$", try_to_do_a_replace, old_text, flags=re.MULTILINE) #type: ignore

    return result #type: ignore


def add_semicolon_to_the_end_of_a_statement(input_code: str) -> str:
    result = re.sub(r"(?P<statement_end>\))\n", ");\n", input_code, flags=re.MULTILINE | re.DOTALL) #type: ignore

    def replace_function(match_obj: Any):
        if match_obj.group() is not None:
            # whole_import_statement = match_obj.group()
            value_copy_statement = match_obj.group('value_copy_statement')
            if not value_copy_statement.endswith(";"):
                value_copy_statement += ";"
            return value_copy_statement + "\n"
    result = re.sub(r"(?P<value_copy_statement>(?:.*) *= *(?:.*))(?:\n)", replace_function, result, flags=re.MULTILINE) #type: ignore

    def replace_function2(match_obj: Any):
        if match_obj.group() is not None:
            # whole_import_statement = match_obj.group()
            value_copy_statement = match_obj.group('return_statement')
            if not value_copy_statement.endswith(";"):
                value_copy_statement += ";"
            return value_copy_statement + "\n"
    result = re.sub(r"(?P<return_statement>return (.*))(?:\n)", replace_function2, result, flags=re.MULTILINE) #type: ignore

    return result #type: ignore


def change_namespace_access_method_from_dot_to_double_colon(input_code: str) -> str:
    module_name_list: list[str] = []
    for one in re.finditer(r"^namespace\ (?P<module_name>.*)", input_code, flags=re.MULTILINE):
        module_name_list.append(one.group("module_name"))
    output_code = input_code
    for module_name in module_name_list:
        output_code = output_code.replace(f"{module_name}.", f'{module_name}::')
    return output_code


def handle_print_function(input_code: str) -> str:
    def do_a_replace(match_obj: Any):
        if match_obj.group() is not None:
            content = match_obj.group("content")
            if content is not None:
                if '"' not in content:
                    if content == "":
                        content = r'"\n"'
                    else:
                        content = f"{content}.c_str()"
                elif content.endswith('"'):
                    content = content[:-1] + r'\n"'
            return f"printf({content})"
    return re.sub(r"print\((?P<content>.*)\)", do_a_replace, input_code) #type: ignore


class HeroToCppCompiler:
    def compile_to_cpp_file(self, input_base_folder: str, input_file: str, output_folder: str) -> str:
        # print(input_base_folder)
        # print(output_folder)
        # print(input_file)

        input_code = io_.read(file_path=input_file)
        output_code = input_code

        # comments handling
        output_code = replace_hextag_to_double_slash(output_code)
        output_code = replace_three_quotation_mark_comments_to_empty_string(output_code)
        output_code = replace_import_statement_to_real_code_block(base_dir=input_base_folder, old_text=output_code)
        # output_code = handle_print_function(output_code)

        output_code = add_semicolon_to_the_end_of_a_statement(output_code)
        output_code = change_namespace_access_method_from_dot_to_double_colon(output_code)

        output_code = """
#include <bits/stdc++.h>
using namespace std;
\n""".lstrip() + output_code

        disk.create_a_folder(output_folder)
        output_pure_file_name, _ = disk.get_stem_and_suffix_of_a_file(input_file)
        output_file_path = disk.concatenate_paths(output_folder, output_pure_file_name+".cpp")
        io_.write(file_path=output_file_path, content=output_code)

        return output_file_path

    def compile_to_binary_file(self, input_cpp_file: str, output_binary_file: str) -> str:
        terminal.run(f"""
        g++ {input_cpp_file} -o {output_binary_file}
        """)
        return output_binary_file


if __name__ == "__main__":
    from auto_everything.terminal import Terminal #type: ignore
    terminal = Terminal()
    terminal.run(f"""
    hero run "/home/yingshaoxo/CS/hero/playground/main.hero"
    """.strip())