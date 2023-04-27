#!/usr/bin/env /usr/bin/python3
#!/usr/bin/env /home/yingshaoxo/anaconda3/bin/python3

# Run this to generate bash auto complete script: Tools -- --completion

import json
from platform import architecture
import re

from auto_everything.python import Python #type: ignore
from auto_everything.disk import Disk #type: ignore
from auto_everything.io import IO #type: ignore
from auto_everything.terminal import Terminal, Terminal_User_Interface #type: ignore
python = Python()
disk = Disk()
io_ = IO()
terminal = Terminal()
terminal_user_interface = Terminal_User_Interface()

import hero_to_golang

# def itIsWindows():
#     if os.name == 'nt':
#         return True
#     return False

class Hero():
    def __init__(self):
        self.hero_to_golang_compiler = hero_to_golang.HeroToGolangCompiler()
        self.current_working_directory = disk.get_current_working_directory()

        self.package_json_file_path = disk.join_paths(self.current_working_directory, "package.json")
        self.has_package_json_file = disk.exists(self.package_json_file_path)
        if (self.has_package_json_file):
            self.package_object = json.loads(io_.read(self.package_json_file_path))

        self.output_golang_folder = disk.join_paths(self.current_working_directory, "build", "golang")
        self.output_golang_mod_path = disk.join_paths(self.output_golang_folder, "go.mod")

        self.binary_output_folder = disk.join_paths(self.current_working_directory, "build", "binary")

    def hi(self):
        print("""
        Hi!\n\nThis is the Hero Programming Language that was made by yingshaoxo.
        """.strip())
    
    def bye(self):
        """
        Uninstall the hero command line tool immediately
        """
        pass

    def run(self, file: str | None = None):
        """
        file: 
            Code file, like 'xx.hero'
        """
        output_folder = disk.get_a_temp_folder_path()

        if file == None and self.has_package_json_file:
            file = self.package_object.get("main")
            output_folder = self.output_golang_folder
        elif file != None and self.has_package_json_file:
            print(f"Sorry, you should use `hero run`")
            return

        if file == None:
            print(f"""Sorry, package.json doesn't have an entry point setting, it should have a pair like {"main": "main.hero"}""")
            return

        file = disk.get_absolute_path(path=file)
        directory_path = disk.get_directory_path(path=file)

        disk.create_a_folder(output_folder)

        golang_file_path = self.hero_to_golang_compiler.compile_to_golang_file(input_base_folder=directory_path, input_file=file, output_folder=output_folder)
        binary_file_path = self.hero_to_golang_compiler.compile_to_binary_file(input_go_file=golang_file_path, output_binary_file=disk.get_a_temp_file_path("hero_run"))
        terminal.run(f"""
        #clear
        echo ""
        echo "cd {disk.get_directory_path(golang_file_path)}"
        echo "vim {golang_file_path}"
        echo ""
        echo "_________________________"
        echo ""
        echo ""
        {binary_file_path}
        """)

    def compile(self, file: str | None = None, platform: str | None = None, output: str | None = None):
        """
        file: 
            Code file, like 'xx.hero'
        platform:
            Operation system and archtecture, like 'linux/amd64' or 'linux/arm'
            -->      darwin/386
            -->    darwin/amd64
            -->       linux/386
            -->     linux/amd64
            -->       linux/arm
            -->     freebsd/386
            -->   freebsd/amd64
            -->     openbsd/386
            -->   openbsd/amd64
            -->     windows/386
            -->   windows/amd64
            -->     freebsd/arm
            -->      netbsd/386
            -->    netbsd/amd64
            -->      netbsd/arm
            -->       plan9/386
        output:
            The output executable file, like `./xx.run`
        """
        """
        You can try to use gox to do the same thing: https://github.com/mitchellh/gox
        """
        output_folder = disk.get_a_temp_folder_path()

        build_based_on_package_json = False
        if file == None and self.has_package_json_file:
            file = self.package_object.get("main")
            output_folder = self.output_golang_folder
            output = self.binary_output_folder
            build_based_on_package_json = True
        elif file != None and self.has_package_json_file:
            print(f"Sorry, you should use `hero compile`")
            return

        disk.create_a_folder(output_folder)

        if output == None:
            print(f"Sorry, you have to specify the output_folder like this: hero compile --file 'xx.hero' --output 'xx.run'")
            return

        if file == None:
            print(f"""Sorry, package.json doesn't have an entry point setting, it should have a pair like {"main": "main.hero"}""")
            return
        file = disk.get_absolute_path(path=file)
        directory_path = disk.get_directory_path(path=file)
        pure_name_of_file, _ = disk.get_stem_and_suffix_of_a_file(file)

        golang_file_path = self.hero_to_golang_compiler.compile_to_golang_file(input_base_folder=directory_path, input_file=file, output_folder=output_folder)

        if platform != None:
            operation_system, architecture = platform.split("/")
        else:
            operation_system, architecture = None, None

        if build_based_on_package_json == True:
            if platform == None:
                output_binary_file = disk.join_paths(output, f"{pure_name_of_file}.run")
            else:
                output_binary_file = disk.join_paths(output, f"{pure_name_of_file}_{operation_system}_{architecture}.run")
        else:
            output_binary_file = disk.get_absolute_path(output)

        binary_file_path = self.hero_to_golang_compiler.compile_to_binary_file(input_go_file=golang_file_path, output_binary_file=output_binary_file, operation_system=operation_system, architecture=architecture)

        terminal.run(f"""
        #clear
        echo ""
        echo "cd {disk.get_directory_path(golang_file_path)}"
        echo "vim {golang_file_path}"
        echo ""
        echo "_________________________"
        echo ""
        echo ""
        echo "{binary_file_path} "
        echo ""
        """)

    def convert(self, file: str, to: str):
        """
        file: 
            Code file, like 'xx.hero'
        to:
            Programming language, like 'golang', 'typescript', 'python', 'cpp', 'dart', 'kotlin', 'C#', 'C++', 'rust'
        """
        pass

    def init(self):
        """
        Create a template by using terminal UI, so that user can add/remove packages
        """
        if disk.exists(self.package_json_file_path):
            print(f"Sorry, folder '{self.current_working_directory}' is not empty. It already has a 'package.json' file.")
            return
        else:
            # json_content = """
            # {
            #     "name": "it_has_alternatives",
            #     "version": "0.0.0",
            #     "scripts": {
            #         "dev": "vite",
            #         "build": "vue-tsc && vite build",
            #     },
            #     "dependencies": {
            #         "vue": "^3.2.47",
            #     },
            #     "devDependencies": {
            #         "vite": "^4.2.0",
            #     }
            # }
            # """
            json_content = """
            {
                "name": "",
                "version": "",
                "author": "",
                "scripts": {},
                "main": "main.hero",
                "dependencies": {},
                "dev_dependencies": {},
                "go_dependencies": {},
                "go_dev_dependencies": {}
            }
            """
            package_object = json.loads(json_content)
            # pprint(package_object)
            print("\n")

            # handle project name
            default_project_name = disk.get_directory_name(self.current_working_directory)
            def assign_name(text: str):
                package_object["name"] = text
            terminal_user_interface.input_box(
                f"Please give me a project name: (use '{default_project_name}' by default)___", 
                default_value=default_project_name,
                handle_function=assign_name
            )

            # handle project version
            default_project_version = "0.0.0"
            def assign_version(text: str):
                package_object["version"] = text
            terminal_user_interface.input_box(
                f"Please give me a project version: (use '{default_project_version}' by default)___", 
                default_value=default_project_version,
                handle_function=assign_version
            )

            # handle author name
            default_author = ""
            def assign_author(text: str):
                package_object["author"] = text
            terminal_user_interface.input_box(
                f"Please give me your name: (It's better be an email address)___", 
                default_value=default_author,
                handle_function=assign_author
            )

            io_.write(self.package_json_file_path, json.dumps(package_object, indent=4))

    def install(self):
        """
        Install all package defined in package.json
        """
        if not self.has_package_json_file:
            print("You have to run `hero init` first.")
            return

        go_dependencies = self.package_object.get("go_dependencies")
        if go_dependencies == None:
            print('package.json should have a pair called {"go_dependencies":{}}')
            return
        
        for package_name in self.package_object.get("go_dependencies").keys(): #type: ignore
            result = terminal.run_command(f"""go get {package_name}""", cwd=self.output_golang_folder)
            print(result)
    
    def add(self, package_name: str, git_link: str | None = None):
        """
        Add a package to project:
            `hero add github.com/google/uuid`
        """
        if not self.has_package_json_file:
            print("You have to run `hero init` first.")
            return

        go_dependencies = self.package_object.get("go_dependencies")
        if go_dependencies == None:
            print('package.json should have a pair called {"go_dependencies":{}}')
            return
        
        result = terminal.run_command(f"""
        go get {package_name}
        """, cwd=self.output_golang_folder)
        if ": EOF".lower() in result.lower():
            print(result)
            return
        else:
            self.package_object['go_dependencies'][package_name] = ""
            io_.write(self.package_json_file_path, json.dumps(self.package_object, indent=4))
        
    def remove(self, package_name: str):
        """
        Remove a package from project:
            `hero remove github.com/google/uuid`
        """
        """
        You could add a command line interface here, if package_name == None, let user choose what to delete
        """
        if not self.has_package_json_file:
            print("You have to run `hero init` first.")
            return

        go_dependencies = self.package_object.get("go_dependencies")
        if go_dependencies == None:
            print('package.json should have a pair called {"go_dependencies":{}}')
            return

        if disk.exists(self.output_golang_mod_path):
            def try_to_do_a_replace(match_obj: re.Match): #type: ignore
                if match_obj.group() is not None:
                    package_require_line = match_obj.group('package_require_line') #type: ignore
                    if package_require_line != None:
                        return "" 
            golang_mod_file_content = io_.read(self.output_golang_mod_path)
            result = re.sub(f"(?P<package_require_line>require {package_name}(?:.*)\s*)", try_to_do_a_replace, golang_mod_file_content, flags=re.MULTILINE) #type: ignore
            io_.write(self.output_golang_mod_path, result) #type: ignore

        result = terminal.run_command(f"""
        go clean
        go mod tidy
        """, cwd=self.output_golang_folder)
        if ": EOF".lower() in result.lower():
            print(result)
            return
        else:
            if package_name in self.package_object['go_dependencies']:
                del self.package_object['go_dependencies'][package_name]
                io_.write(self.package_json_file_path, json.dumps(self.package_object, indent=4))
            else:
                print(f"Sorry, package '{package_name}' not in:")
                print("".join([f"* {one}\n" for one in self.package_object['go_dependencies'].keys()]))
                return

    # def update(self, package_name: str, git_link: str | None = None):
    #     """
    #     Update a package for this project
    #     """
    #     pass

    # def update_all(self):
    #     """
    #     Update all package for this project
    #     """
    #     pass


python.make_it_global_runnable(executable_name="hero")
python.fire(Hero)
