import re
from typing import Any, Tuple
import json
import platform
import os

from auto_everything.disk import Disk #type: ignore
from auto_everything.io import IO #type: ignore
from auto_everything.terminal import Terminal #type: ignore
from auto_everything.string import String #type: ignore
disk = Disk()
io_ = IO()
terminal = Terminal()
string_tool = String()


def my_capitalize_function(text: str) -> str:
    if len(text) == 0:
        return text
    return text[0].capitalize() + text[1:]


class HeroToGolangCompiler:
    def __init__(self):
        self.keywords_list = [
            'export',
            'return',
            'if',
            'package'
        ]

        self.current_working_directory = disk.get_current_working_directory()

        self.package_json_file_path = disk.join_paths(self.current_working_directory, "package.json")
        self.has_package_json_file = disk.exists(self.package_json_file_path)
        if (self.has_package_json_file):
            self.package_object = json.loads(io_.read(self.package_json_file_path))
            self.go_package_name = self.package_object.get("go_package_name")
            if self.go_package_name == None:
                self.go_package_name = ""

        self.output_golang_folder = disk.join_paths(self.current_working_directory, "build", "golang")
        self.output_golang_mod_path = disk.join_paths(self.output_golang_folder, "go.mod")

        self.binary_output_folder = disk.join_paths(self.current_working_directory, "build", "binary")

        self.git_user_email = terminal.run_command("git config user.email").strip().split('\n')[0]
        self.git_user_name: str = terminal.run_command("git config user.name").strip().split('\n')[0].lower()
        if self.git_user_name == "":
            self.git_user_name = string_tool.remove_all_special_characters_from_a_string(os.getlogin(), white_list_characters='_').strip().lower()
            if self.git_user_name == "":
                self.git_user_name = platform.system().lower().strip()

    def replace_hextag_to_double_slash(self, old_text: str) -> str:
        def do_a_replace(match_obj: Any):
            if match_obj.group() is not None:
                original_line = match_obj.group()
                original_line = original_line.replace("#", "//", 1)
                return original_line

        result = re.sub(r"[ \t]*#(?:.*)", do_a_replace, old_text) #type: ignore
        return result #type: ignore

    def replace_three_quotation_mark_to_three_slash(self, old_text: str) -> str:
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

    def replace_three_quotation_mark_comments_to_empty_string(self, old_text: str) -> str:
        result = re.sub(r"([ \t]*\"\"\"(?:\s.*?)*\"\"\")", "", old_text) #type: ignore
        result = re.sub(r"([ \t]*\'\'\'(?:\s.*?)*\'\'\')", "", result) #type: ignore
        return result #type: ignore
    
    def convert_hero_variable_type_into_golang_variable_type(self, old_type: str) -> str:
        type_map = {
            'int': 'int64',
            'float': 'float64'
        }
        if old_type in type_map.keys():
            return type_map[old_type]
        else:
            return old_type
    
    def convert_hero_function_input_arguments_into_golang_function_input_arguments(self, old_text: str) -> str:
        def try_to_do_a_replace(match_obj: re.Match): #type: ignore
            if match_obj.group() is not None:
                variable_type = str(match_obj.group('variable_type')) #type: ignore
                variable_name = match_obj.group('variable_name') #type: ignore

                variable_type = self.convert_hero_variable_type_into_golang_variable_type(variable_type)

                golang_input_arguments_template = f"""{variable_name} {variable_type}""".strip()

                return golang_input_arguments_template

        return re.sub(r"(?P<variable_type>[\w\[\]\.\*]+) (?P<variable_name>\w+)", try_to_do_a_replace, old_text, flags=re.MULTILINE) #type: ignore

    def convert_hero_package_function_access_method_to_golang_package_function_access_method(self, old_text: str, package_name_list: list[str]) -> str:
        def part2_replacement(text: str) -> str:
            def re_part2_replacement(match_obj: re.Match): #type: ignore
                if match_obj.group() is not None:
                    function_name = match_obj.group('function_name') #type: ignore
                    return f".{my_capitalize_function(function_name)}(" #type: ignore
            text = re.sub(f"\.(?P<function_name>\w+)\(", re_part2_replacement, text, flags=re.MULTILINE) #type: ignore
            return text #type: ignore

        for package_name in package_name_list:
            def try_to_do_a_replace(match_obj: re.Match): #type: ignore
                if match_obj.group() is not None:
                    part1 = match_obj.group('part1') #type: ignore
                    part2 = match_obj.group('part2') #type: ignore
                    return part1 + part2_replacement(part2) #type: ignore
            old_text = re.sub(f"(?P<part1>{package_name})(?P<part2>(?:\s*\.\w+\((?:(?:\s|.)*?)+\))+)", try_to_do_a_replace, old_text, flags=re.MULTILINE) #type: ignore
        return str(old_text) #type: ignore
    
    def change_hero_function_into_golang_function_format_by_using_regex_in_a_unsafe_way(self, old_text: str) -> str:
        def try_to_do_a_replace(match_obj: re.Match): #type: ignore
            if match_obj.group() is not None:
                should_i_export_it = bool(match_obj.group("export") != None) #type: ignore
                is_it_async_function = bool(match_obj.group("async") != None) #type: ignore
                function_name = str(match_obj.group('function_name')) #type: ignore
                function_input_arguments = str(match_obj.group('function_input_arguments')) #type: ignore
                function_content = match_obj.group('function_content').strip('\n') #type: ignore
                function_return_type = str(match_obj.group('function_return_type')) #type: ignore

                if should_i_export_it == True:
                    function_name = my_capitalize_function(function_name)

                function_input_arguments = self.convert_hero_function_input_arguments_into_golang_function_input_arguments(function_input_arguments)

                if function_return_type == 'void':
                    function_return_type = ''
                if function_name.lower() == 'main':
                    function_return_type = ''
                function_return_type = self.convert_hero_variable_type_into_golang_variable_type(function_return_type) #type: ignore

                golang_function_template = f"""
func {function_name}({function_input_arguments}) {function_return_type} {{
{function_content}
}}
                """.strip()

                return golang_function_template

        return re.sub(r"^(?P<export>export )*(?P<async>async )*(?P<function_return_type>.*) (?P<function_name>.*)\((?P<function_input_arguments>.*)\) \{(?P<function_content>(?:\n|.)*?)\}", try_to_do_a_replace, old_text, flags=re.MULTILINE) #type: ignore

    def find_next_code_block(self, text: str, start: int = 0) -> Tuple[int|None, int|None]:
        search_balanced_curly_brackets_end = False
        start_index = None
        end_index = None
        curly_brackets_start_symbol_count = 0
        curly_brackets_end_symbol_count = 0
        for index, char in enumerate(text):
            if index < start:
                continue
            if search_balanced_curly_brackets_end == False:
                if char == "{":
                    search_balanced_curly_brackets_end = True
                    start_index = index
                    curly_brackets_start_symbol_count += 1
                    continue
            if char == "{":
                curly_brackets_start_symbol_count += 1
                continue
            if search_balanced_curly_brackets_end == True:
                if char == "}":
                    curly_brackets_end_symbol_count += 1
                    if curly_brackets_start_symbol_count == curly_brackets_end_symbol_count:
                        end_index = index
                        break
        return start_index, end_index+1 #type: ignore

    def change_hero_function_into_golang_function_format(self, old_text: str) -> str:
        def try_to_do_a_replace(match_obj: re.Match): #type: ignore
            if match_obj.group() is not None:
                should_i_export_it = bool(match_obj.group("export") != None) #type: ignore
                is_it_async_function = bool(match_obj.group("async") != None) #type: ignore
                function_name = str(match_obj.group('function_name')) #type: ignore
                function_input_arguments = str(match_obj.group('function_input_arguments')) #type: ignore
                function_content = match_obj.group('function_content')#.strip('\n') #type: ignore
                function_return_type = str(match_obj.group('function_return_type')) #type: ignore

                if should_i_export_it == True:
                    function_name = my_capitalize_function(function_name)

                function_input_arguments = self.convert_hero_function_input_arguments_into_golang_function_input_arguments(function_input_arguments)

                if function_return_type == 'void':
                    function_return_type = ''
                if function_name.lower() == 'main':
                    function_return_type = ''
                function_return_type = self.convert_hero_variable_type_into_golang_variable_type(function_return_type) #type: ignore

                if function_return_type.strip() == "":
                    golang_function_template = f"""
    func {function_name}({function_input_arguments}) {{{function_content}}}
                    """.strip()
                else:
                    golang_function_template = f"""
    func {function_name}({function_input_arguments}) {function_return_type} {{{function_content}}}
                    """.strip()

                return golang_function_template

        output_text = re.sub(r"^(?P<export>export )*(?P<async>async )*(?P<function_return_type>.*) (?P<function_name>.*)\((?P<function_input_arguments>.*)\) \{(?P<function_content>(?:\n|.)*?)\}", try_to_do_a_replace, old_text, flags=re.MULTILINE) #type: ignore

        # # get real code_block if needed
        # content_start_and_end_index_list: list[Tuple[int, int]] = []
        # new_function_match_list = re.finditer(r"^func (?P<function_name>.*)\((?P<function_input_arguments>.*)\) \{(?P<function_content>(?:\n|.)*?)\}", output_text, flags=re.MULTILINE) #type: ignore
        # for match in new_function_match_list:
        #     if match.group() is not None: #type: ignore
        #         function_name_start_index = match.start('function_name') #type: ignore
        #         if function_name_start_index != -1:
        #             code_block_start, code_block_end = self.find_next_code_block(output_text, start=function_name_start_index) #type: ignore
        #             if code_block_start != None and code_block_end != None:
        #                 code_block = output_text[code_block_start:code_block_end] #type: ignore
        #                 content_start_and_end_index_list.append((code_block_start,code_block_end))
        #                 print(code_block) #type: ignore
        #                 print("\n\n--------------\n\n")

        return output_text #type: ignore
    
    def convert_hero_variable_definition_to_golang_variable_definition(self, old_text: str) -> str:
        def try_to_do_a_replace_1(match_obj: Any):
            if match_obj.group() is not None:
                indent = match_obj.group('indent')
                variable_type = match_obj.group('variable_type')
                variable_name = match_obj.group('variable_name')
                if variable_type in self.keywords_list:
                    return f'{indent}{variable_type} {variable_name} ='
                else:
                    return f'{indent}var {variable_name} {variable_type} ='
        result = re.sub(r"^(?P<indent>[ \t]*)(?P<variable_type>[\w\[\]\.]+) (?P<variable_name>\w+) =", try_to_do_a_replace_1, old_text, flags=re.MULTILINE) #type: ignore

        def try_to_do_a_replace_2(match_obj: Any):
            if match_obj.group() is not None:
                indent = match_obj.group('indent')
                variable_type = match_obj.group('variable_type')
                variable_name = match_obj.group('variable_name')
                if variable_type in self.keywords_list:
                    return f'{indent}{variable_type} {variable_name}'
                else:
                    return f'{indent}var {variable_name} {variable_type}'
        result = re.sub(r"^(?P<indent>[ \t]*)(?P<variable_type>[\w\[\]\.]+) (?P<variable_name>\w+)$", try_to_do_a_replace_2, result, flags=re.MULTILINE) #type: ignore

        return result #type: ignore
    
    def handle_import_statement(self, base_dir: str, old_text: str, output_folder: str, parent_package_path: str | None = None) -> Tuple[str, list[str]]:
        if parent_package_path == None or parent_package_path == "":
            print("If you want to use `import` feature, you have to use `hero init` to create a project first.")
            exit()

        package_name_list: list[str] = []

        def try_to_do_a_replace(match_obj: Any):
            if match_obj.group() is not None:
                module_name = match_obj.group('name')
                module_nickname = match_obj.group('nickname')

                module_path = module_name
                if module_name.startswith("./") or module_name.startswith("/"):
                    # it is a file path
                    if module_nickname != None:
                        module_name = module_nickname
                    else:
                        module_name, _ = disk.get_stem_and_suffix_of_a_file(module_name)
                    package_name_list.append(module_name)
                else:
                    # it is go package or hero package
                    # if it is hero package: you need to find the real path from 'hero_modules' folder
                    if module_nickname == None:
                        module_nickname = module_name
                    package_name_list.append(module_nickname)
                    return f'import {module_nickname} "{module_name}"'

                file = disk.get_absolute_path(path=module_path)
                directory_path = disk.get_directory_path(path=file)
                self.compile_to_golang_file(input_base_folder=directory_path, input_file=file, output_folder=output_folder, parent_package_path=parent_package_path)
                pure_module_file_name, _ = disk.get_stem_and_suffix_of_a_file(path=module_path)
                
                return f'import {module_name} "{parent_package_path}/{pure_module_file_name}"'

        result = re.sub(r"^import \"(?P<name>.*)\"(?: as (?P<nickname>.*))*$", try_to_do_a_replace, old_text, flags=re.MULTILINE) #type: ignore

        return result, package_name_list #type: ignore

    def compile_to_golang_file(self, input_base_folder: str, input_file: str, output_folder: str, parent_package_path: str | None = None) -> str:
        # print(input_base_folder)
        # print(output_folder)
        # print(input_file)
        disk.create_a_folder(output_folder)
        output_pure_file_name, _ = disk.get_stem_and_suffix_of_a_file(input_file)
        output_file_path = disk.concatenate_paths(output_folder, output_pure_file_name+".go")

        if self.has_package_json_file == True:
            if parent_package_path == None:
                parent_package_path = f"{self.go_package_name}"
        else:
            package_hash = disk.get_hash_of_a_path(disk.get_absolute_path(input_file))[:15]
            package_path = f"{package_hash}/{output_pure_file_name}"
            go_mod_path = disk.join_paths(output_folder, "go.mod")
            if not disk.exists(go_mod_path):
                terminal.run(f"""
                export GO111MODULE=on
                go mod init {package_path}
                """, cwd=output_folder)
            parent_package_path = package_path

        input_code = io_.read(file_path=input_file)
        output_code = input_code

        # comments handling
        output_code = self.replace_hextag_to_double_slash(output_code)
        output_code = self.replace_three_quotation_mark_comments_to_empty_string(output_code)
        output_code = self.change_hero_function_into_golang_function_format(old_text=output_code)
        output_code, package_name_list = self.handle_import_statement(base_dir=input_base_folder, old_text=output_code, output_folder=output_folder, parent_package_path=parent_package_path)
        output_code = self.convert_hero_package_function_access_method_to_golang_package_function_access_method(old_text=output_code, package_name_list=package_name_list)
        output_code = self.convert_hero_variable_definition_to_golang_variable_definition(old_text=output_code)

        output_code = f"""
// stupid golang doesn't allow unused variable exists, which is bad for coding experience
// proposal: spec: make unused variables not an error #43729: https://github.com/golang/go/issues/43729

package {output_pure_file_name}\n\n\n""".lstrip() + output_code


        # if package_nickname == None:
        #     output_code = f"""package {output_pure_file_name}\n\n\n""" + output_code
        # else:
        #     output_code = f"""package {package_nickname}\n\n\n""" + output_code

        sub_folder = disk.join_paths(output_folder, output_pure_file_name) 
        disk.create_a_folder(sub_folder)
        output_file_path = disk.join_paths(sub_folder, output_pure_file_name + ".go")
        io_.write(file_path=output_file_path, content=output_code)

        return output_file_path

    def compile_to_binary_file(self, input_go_file: str, output_binary_file: str, operation_system: str | None = None, architecture: str | None = None) -> str:
        base_folder = disk.get_directory_path(input_go_file)

        if operation_system != None and architecture != None:
            result = terminal.run_command(f"""
            export GOOS={operation_system} 
            export GOARCH={architecture}
            export GO111MODULE=on
            go build -o {output_binary_file} {input_go_file} 
            """, cwd=base_folder, timeout=999999)
        else:
            result = terminal.run_command(f"""
            export GO111MODULE=on
            go build -o {output_binary_file} {input_go_file} 
            """, cwd=base_folder, timeout=999999)
        result = result.replace("go get ", "hero add ")
        print(result)

        return output_binary_file



if __name__ == "__main__":
    from auto_everything.terminal import Terminal #type: ignore
    terminal = Terminal()
    terminal.run(f"""
    hero run "/home/yingshaoxo/CS/hero/playground/0.first_dev_phase/main.hero"
    """.strip())