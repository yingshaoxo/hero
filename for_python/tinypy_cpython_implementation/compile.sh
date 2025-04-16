cd tinypy

#musl-gcc $WFLAGS -O3 mymain.c $FLAGS -lm -o ../boot_py
musl-gcc -g0 -s -std=c99 -static -D_POSIX_SOURCE -no-pie -o ../boot_py mymain.c
