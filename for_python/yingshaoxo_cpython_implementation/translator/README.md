# Ypython Translator

Another version of python. Because it is made by yingshaoxo, that is why it is called "ypython".

> A thing to mention is: **Why python is 60 times slower than c code?** Because for each variable, for example, string variable, it has more data structure than char*. And when we parse the python code, it takes time.

## Goal
### 1. First write a simple python executer with python3.10.4 or python3.2. It can run simple python script. Let's call it "y_python.py". You use it by "python3 y_python.py test.py"
```
# test.py

parent_variable = "parent"
print(parent_variable)

def a_function_1():
    a_child_variable = "whatever"
    print(parent_variable)
    print(a_child_variable)

a_function_1()
print(a_child_variable)

def a_function_2(temp_2, temp3):
    temp_1 = " you say"
    a_child_variable2 = "whatever" + temp_1
    print(a_child_variable2)
    print(temp_2)
    print(temp3)

a_function_2("nice", temp3="yeah")
print("is" + " right")

a_dict = {a: 3}
print(a_dict)

a_list = ["god", "yingshaoxo"]
print(a_list)

print(2 + 3)

a_type = type(2)
print(a_type)

def a_function_3(number_1, number_2):
    return number_1 + number_2

result1 = a_function_3(number_1=6, number_2=7)
print(result1)

long_text = """
hi you,
    dear.
"""

print(long_text)


a2 = 1
b2 = 1
print(a2 == b2)
if a2 == b2:
    print("a2 and b2 is equal")

while a2 < 7:
    print(a2)
    a2 = a2 + 1
    if a2 == 4:
        break

class A_Class():
    def __init__(self):
        print("class instance creating...")
        self.pre_defined_variable = "variable created in class creation"

    def hi(self):
        temp = self.pre_defined_variable
        print(temp)
        print("yingshaoxo:")

    def hi2(self, words):
        print(words)
        return "you"

    def hi3(self):
        self.a_variable = 222
        local_variable = 666

    def hi4(self):
        a_test2 = self.a_variable
        a_test2 = a_test2 + 1
        print(a_test2)
        print(local_variable)

a_class = A_Class()
a_class.hi()
result = a_class.hi2(words="hi")
print(result)

a_class.hi3()
result2 = a_class.a_variable
print(result2)

a_class.hi4()
```

```
# test.py result

parent
parent
whatever
Error: no variable called 'a_child_variable'
whatever you say
nice
yeah
is right
a: 3
[god, yingshaoxo]
5
int
13
hi you,
    dear.
True
a2 and b2 is equal
1
2
3
class instance creating...
variable created in class creation
yingshaoxo:
hi
you
222
223
Error: no variable called 'local_variable'
```

### 2. Then write another python script that could parse y_python.py, and convert it into c99 code. If you can convert it to golang or vlang, it is also OK. But you have to make sure the final compiled program is runable in 2005 years old linux, for example, ubuntu8. Let's call the final compiled program "y_python.run".

It is similar to what cython_to_c did, but the base c library is y_python.h

### 3. Now, upgrade "y_python.py" to let it has the ability to convert python code into c99. So you can use it like "./y_python.run test.py -o test.c" or "./y_python.run test.py"

Where "./y_python.run test.py -o test.c" can output a c99 version of the text.py file.

Where "./y_python.run test.py" can parse and run the test.py file in real time.

### 4. You are actually getting rid of the original python dependencies, because the original python is at least 100 MB bigger, but your new program "y_python.run" only has 512KB and can convert many simple python script into c99 code. Including itself, I mean, the original "y_python.py".

### 5. Then you have to consider where can you get a static version of gcc or tinycc for linux, so that you can compile c99 code in 2005 year old ubuntu8 linux system without network.
