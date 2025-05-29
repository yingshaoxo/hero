name = "Hi"
name = "yingshaoxo"
print(name)
#assert name == "yingshaoxo"

def a_function():
    new_variable = "nice"
    print(new_variable)
    print(name)
    print("\n\n")

a_function()

def a_function2():
    no_way = "haha"
    a_number = 12345
    a_float_number = 2.3
    print(no_way)
    print(a_number)
    print(a_float_number)

a_function2()

try:
    print(an_error_of_not_exists)
    print("will not print")
except Exception as e:
    print("try and except works")

if 1 == 2:
    function3()

if 2 == 2:
    print("if works for literal characters")


if name == "yingshaoxo":
    print("if works for variables")

if name == "shit":
    print("if not work for variables")
