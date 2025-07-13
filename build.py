import os
import platform

def build_mac():
    build = "gcc test.c -std=c17 -Wall -o test"
    os.system(build)
    os.system("./test")
    os.remove("./test")

def build_win():
    build = "gcc test.c -std=c17 -Wall -o test.exe"
    os.system(build)
    os.system("test.exe")
    os.remove("test.exe")

# python build.py
if platform.system() == "Windows":
    build_win()
else:
    build_mac()
