import re
from typing import Any, Tuple

from auto_everything.disk import Disk #type: ignore
from auto_everything.io import IO #type: ignore
from auto_everything.terminal import Terminal #type: ignore
disk = Disk()
io_ = IO()
terminal = Terminal()


class HeroToGolangCompiler:
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

        return re.sub(r"(?P<variable_type>\w+) (?P<variable_name>\w+)", try_to_do_a_replace, old_text, flags=re.MULTILINE) #type: ignore

    def convert_hero_package_function_access_method_to_golang_package_function_access_method(self, old_text: str, package_name_list: list[str]) -> str:
        for package_name in package_name_list:
            def try_to_do_a_replace(match_obj: re.Match): #type: ignore
                if match_obj.group() is not None:
                    part1 = match_obj.group('part1') #type: ignore
                    part2 = match_obj.group('part2') #type: ignore
                    return part1 + str(part2).capitalize() #type: ignore
            old_text = re.sub(f"(?P<part1>{package_name}\.\s*)(?P<part2>\w)", try_to_do_a_replace, old_text, flags=re.MULTILINE) #type: ignore
        return str(old_text) #type: ignore
    
    def change_hero_function_into_golang_function_format(self, old_text: str) -> str:
        def try_to_do_a_replace(match_obj: re.Match): #type: ignore
            if match_obj.group() is not None:
                should_i_export_it = bool(match_obj.group("export") != None) #type: ignore
                is_it_async_function = bool(match_obj.group("async") != None) #type: ignore
                function_name = str(match_obj.group('function_name')) #type: ignore
                function_input_arguments = str(match_obj.group('function_input_arguments')) #type: ignore
                function_content = match_obj.group('function_content').strip('\n') #type: ignore
                function_return_type = str(match_obj.group('function_return_type')) #type: ignore

                if should_i_export_it == True:
                    function_name = function_name.capitalize()

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
    
    def handle_import_statement(self, base_dir: str, old_text: str, output_folder: str, parent_package_path: str | None = None) -> Tuple[str, list[str]]:
        package_name_list: list[str] = []

        def try_to_do_a_replace(match_obj: Any):
            if match_obj.group() is not None:
                module_name = match_obj.group('name')
                module_nickname = match_obj.group('nickname')

                module_path = module_name
                if module_name.startswith("./") or module_name.startswith("/"):
                    # it is a file path
                    if module_name.startswith("/"):
                        # it is an absolute path, ignore it
                        pass
                    # else:
                    #     module_path = disk.join_relative_paths(base_dir, module_path)
                else:
                    # it is go package or hero package
                    # if it is hero package: you need to find the real path from 'hero_modules' folder
                    return f'import "{module_name}"'

                if module_nickname != None:
                    module_name = module_nickname
                else:
                    module_name, _ = disk.get_stem_and_suffix_of_a_file(module_name)
                package_name_list.append(module_name)
                
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

        if parent_package_path == None:
            # create a package
            package_hash = disk.get_hash_of_a_path(disk.get_absolute_path(input_file))[:15]
            package_path = f"{package_hash}/{output_pure_file_name}"
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

        output_code = f"""package {output_pure_file_name}\n\n\n""" + output_code
        # if package_nickname == None:
        #     output_code = f"""package {output_pure_file_name}\n\n\n""" + output_code
        # else:
        #     output_code = f"""package {package_nickname}\n\n\n""" + output_code

        sub_folder = disk.join_paths(output_folder, output_pure_file_name) 
        disk.create_a_folder(sub_folder)
        output_file_path = disk.join_paths(sub_folder, output_pure_file_name + ".go")
        io_.write(file_path=output_file_path, content=output_code)

        return output_file_path

    def compile_to_binary_file(self, input_go_file: str, output_binary_file: str) -> str:
        base_folder = disk.get_directory_path(input_go_file)
        terminal.run(f"""
        export GO111MODULE=on
        go build -o {output_binary_file} {input_go_file} 
        """, cwd=base_folder)
        return output_binary_file



if __name__ == "__main__":
    from auto_everything.terminal import Terminal #type: ignore
    terminal = Terminal()
    terminal.run(f"""
    hero run "/home/yingshaoxo/CS/hero/playground/0.first_dev_phase/main.hero"
    """.strip())