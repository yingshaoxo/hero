# generate c code 
cython --embed -3 y_python.py -o ./y_python_cython.c  

# compile c to *.run  
# Whenever you want to compile the c file it generated, you have to be able to compile the original python code first.
# even if you did, if you do not add -static to that command, your compiled file will only be able to get running in your own computer
rm ./dist/y_python.run
gcc -std=c99 -lpthread $(python3-config --includes) $(python3-config --ldflags) ./y_python_cython.c -o ./dist/y_python.run


# It is not easy to do the static compile in normal linux distrubution
# -I/usr/include/python3.7m -I/usr/include/python3.7m
# -L/usr/lib/python3.7/config-3.7m-x86_64-linux-gnu -L/usr/lib -lpython3.7m -lcrypt -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions
#gcc -std=c99 -static -lpthread -I /home/python/use_docker_to_build_static_python3_binary_executable/data/Python-3.10.4/ -I /home/python/use_docker_to_build_static_python3_binary_executable/data/Python-3.10.4/Include ./dist/y_python.c /home/python/use_docker_to_build_static_python3_binary_executable/data/Python-3.10.4/libpython3.10.a -lpthread -lm -ldl -lz -lutil -lgdbm  -o ./dist/y_python.run
 


echo "./dist/y_python.run"


echo -e "\n\n\n"
echo "The following is the output of test.py:"
./dist/y_python.run test.py
