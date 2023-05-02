#!/usr/bin/env /usr/bin/python3
#!/usr/bin/env /home/yingshaoxo/anaconda3/bin/python3

# Run this to generate bash auto complete script: Tools -- --completion

import json
import re
import os
import platform

from auto_everything.python import Python #type: ignore
from auto_everything.disk import Disk #type: ignore
from auto_everything.io import IO #type: ignore
from auto_everything.terminal import Terminal, Terminal_User_Interface #type: ignore
from auto_everything.string import String #type: ignore
python = Python()
disk = Disk()
io_ = IO()
terminal = Terminal()
terminal_user_interface = Terminal_User_Interface()
string_tool = String()

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
            self.go_package_name = self.package_object.get("go_package_name")
            if self.go_package_name == None:
                self.go_package_name = ""

        self.output_golang_folder = disk.join_paths(self.current_working_directory, "build", "golang")
        self.output_golang_mod_path = disk.join_paths(self.output_golang_folder, "go.mod")

        self.binary_output_folder = disk.join_paths(self.current_working_directory, "build", "binary")

        self.git_user_email = terminal.run_command("git config user.email").strip().split('\n')[0]
        self.git_user_name = terminal.run_command("git config user.name").strip().split('\n')[0].lower()
        if self.git_user_name == "":
            self.git_user_name = string_tool.remove_all_special_characters_from_a_string(os.getlogin(), white_list_characters='_').strip().lower()
            if self.git_user_name == "":
                self.git_user_name = platform.system().lower().strip()

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

        build_based_on_package_json = False
        if file == None and self.has_package_json_file:
            file = self.package_object.get("main")
            output_folder = self.output_golang_folder
            build_based_on_package_json = True
        elif file != None and self.has_package_json_file:
            print(f"Sorry, you should use `hero run`")
            exit()
        elif file == None and not self.has_package_json_file:
            print(f"Sorry, you should use `hero run xx.hero`")
            exit()

        if file == None:
            print(f"""Sorry, package.json doesn't have an entry point setting, it should have a pair like {"main": "main.hero"}""")
            exit()

        file = disk.get_absolute_path(path=file)
        directory_path = disk.get_directory_path(path=file)

        disk.create_a_folder(output_folder)

        if build_based_on_package_json == True:
            self._prepare_golang_output_module()

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
            self._prepare_golang_output_module()
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

    def _prepare_golang_output_module(self):
        """
        #todo: reduce unnessesary package online update by doing a check on package.json and go.mod, if they both have same dependencies, ignore the self.install() in this function
        """
        self.__init__()

        disk.create_a_folder(self.output_golang_folder)
        if self.go_package_name == "":
            print("You have to run 'hero init' first")
            exit()
        terminal.run(f"""
        go mod init {self.go_package_name}
        """, cwd=self.output_golang_folder)

        self.__init__()
        self.add('github.com/yingshaoxo/gopython/built_in_functions')

        self.__init__()
        self.install()

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
                "go_package_name": "",
                "go_dependencies": {},
                "go_dev_dependencies": {}
            }
            """
            package_object = json.loads(json_content)
            # pprint(package_object)
            print("\n")

            # handle project name
            default_project_name = disk.get_directory_name(self.current_working_directory)
            print(default_project_name)
            def assign_name(text: str):
                package_object["name"] = text
            terminal_user_interface.input_box(
                f"Please give me a project name: (default '{default_project_name}')___", 
                default_value=default_project_name,
                handle_function=assign_name
            )

            # handle go_package name
            project_name = package_object["name"]
            default_go_package_name = f"github.com/{self.git_user_name}/{project_name}"
            def assign_go_package_name(text: str):
                package_object["go_package_name"] = text
            terminal_user_interface.input_box(
                f"Please give me a go package name: (default '{default_go_package_name}')___", 
                default_value=default_go_package_name,
                handle_function=assign_go_package_name
            )

            # handle project version
            default_project_version = "0.0.0"
            def assign_version(text: str):
                package_object["version"] = text
            terminal_user_interface.input_box(
                f"Please give me a project version: (default '{default_project_version}')___", 
                default_value=default_project_version,
                handle_function=assign_version
            )

            # handle author name
            default_author = self.git_user_email
            if "@" not in default_author:
                default_author = "" 
            def assign_author(text: str):
                package_object["author"] = text
            terminal_user_interface.input_box(
                f"Please give me your name: (default '{default_author}')___", 
                default_value=default_author,
                handle_function=assign_author
            )

            io_.write(self.package_json_file_path, json.dumps(package_object, indent=4))

            main_hero_template = r"""
import "github.com/yingshaoxo/gopython/built_in_functions" as gopython
import "./a_module.hero" as a_module

void main() {
	gopython.print("Hello, world!")

    a_module.run("2333", "/")
}
            """.strip()
            main_hero_file_path = disk.join_paths(self.current_working_directory, "main.hero")
            io_.write(main_hero_file_path, main_hero_template)

            a_module_template = r"""
import (
    "io"
    "net"
    "net/http"
    "net/url"
    "context"
)

string http_join_path(string path1, string path2) {
    s, err := url.JoinPath(path1, path2)
    if err == nil {
        return s
    } else {
        return path1 + path2
    }
}

void home_handler(http.ResponseWriter w, *http.Request r) {
    println("got / request\n")
    io.WriteString(w, "This is your website!\n")
}

void hello_handler(http.ResponseWriter w, *http.Request r) {
    println("got /hello request\n")
    io.WriteString(w, "Hello, HTTP!\n")
}

void author_handler(http.ResponseWriter w, *http.Request r) {
    println("got /author request\n")
    io.WriteString(w, "yingshaoxo\n")
}

export void run(string port, string url_prefix) {
    mux1 := http.NewServeMux()

    mux1.HandleFunc(http_join_path(url_prefix, "/"), home_handler)
    mux1.HandleFunc(http_join_path(url_prefix, "/hello"), hello_handler)
    mux1.HandleFunc(http_join_path(url_prefix, "/author"), author_handler)

    ctx, _ := context.WithCancel(context.Background())
    serverOne := &http.Server{
        Addr:    ":" + port,
        Handler: mux1,
        BaseContext: func(l net.Listener) context.Context {
            return ctx
        },
    }

    println("\n\nYour website is on: http://localhost:" + port + url_prefix)
    serverOne.ListenAndServe()
}

            """.strip()
            a_module_file_path = disk.join_paths(self.current_working_directory, "a_module.hero")
            io_.write(a_module_file_path, a_module_template)

            self._prepare_golang_output_module()

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
