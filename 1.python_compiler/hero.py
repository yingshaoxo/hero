#!/usr/bin/env /usr/bin/python3
#!/usr/bin/env /home/yingshaoxo/anaconda3/bin/python3

# Run this to generate bash auto complete script: Tools -- --completion

from auto_everything.python import Python #type: ignore
from auto_everything.disk import Disk #type: ignore
from auto_everything.io import IO #type: ignore
from auto_everything.terminal import Terminal #type: ignore
python = Python()
disk = Disk()
io_ = IO()
terminal = Terminal()

import hero_to_cpp

# def itIsWindows():
#     if os.name == 'nt':
#         return True
#     return False

class Hero():
    def __init__(self):
        self.hero_to_cpp_compiler = hero_to_cpp.HeroToCppCompiler()

    def hi(self):
        print("""
        Hi!\n\nThis is the Hero Programming Language that was made by yingshaoxo.
        """.strip())
    
    def bye(self):
        """
        Uninstall the hero command line tool immediately
        """
        pass

    def run(self, file: str):
        """
        file: 
            Code file, like 'xx.hero'
        """
        file = disk.get_absolute_path(path=file)
        directory_path = disk.get_directory_path(path=file)

        output_folder = disk.get_a_temp_folder_path()
        # output_folder = "/home/yingshaoxo/CS/hero/playground/run_compiling_output"
        cpp_file_path = self.hero_to_cpp_compiler.compile_to_cpp_file(input_base_folder=directory_path, input_file=file, output_folder=output_folder)
        binary_file_path = self.hero_to_cpp_compiler.compile_to_binary_file(input_cpp_file=cpp_file_path, output_binary_file=disk.get_a_temp_file_path("hero_run"))
        terminal.run(f"""
        #clear
        echo ""
        echo ""
        echo "_________________________"
        echo ""
        echo ""
        {binary_file_path}
        """)

    def compile(self, file: str, platform: str):
        """
        file: 
            Code file, like 'xx.hero'
        platform:
            Operation system and archtecture, like 'linux/amd64' or 'linux/arm'
        """
        pass

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
        pass
    
    def add(self, package_name: str, git_link: str | None = None):
        """
        Add a package to project
        """
        pass

    def remove(self, package_name: str):
        """
        Remove a package from project
        """
        pass


python.make_it_global_runnable(executable_name="hero")
python.fire(Hero)
