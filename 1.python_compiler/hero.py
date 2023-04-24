#!/usr/bin/env /usr/bin/python3
#!/usr/bin/env /home/yingshaoxo/anaconda3/bin/python3

# Run this to generate bash auto complete script: Tools -- --completion

import os, re

from auto_everything.python import Python #type: ignore
from auto_everything.disk import Disk #type: ignore

python = Python()
disk = Disk()

# def itIsWindows():
#     if os.name == 'nt':
#         return True
#     return False

class Hero():
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
        pass

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
