f = open("README.md", "r")
print(f.read())
#f = open("test.txt", "w")
#f.write("yingshaoxo is the best!\n\nyeah!")
f.close()


print("\n\n------\n\n")


class AClass(object):
#class AClass():
    def __init__(self):
        pass

    def print(self, whatever):
        print(whatever)

a_class = AClass()
a_class.print("class works")


print("\n\n------\n\n")


print("'format'" + " works.")


print("\n\n------\n\n")


print(run_command("uname -v"))


print("\n\n------\n\n")

result = input("Do you want to see how many program in your computer that is running? (y/n)")
if "y" in result:
    run("top")
else:
    print("Wait 3 seconds, I will quit.")
    sleep(3)
