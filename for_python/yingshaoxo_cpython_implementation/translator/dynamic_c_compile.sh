# sudo apt install musl-tools

#c_compiler=$(which musl-gcc)
c_compiler=$(which gcc)

mkdir -p dist/
rm -fr dist/*

$c_compiler -g0 -s -std=c99 -static -D_POSIX_SOURCE -no-pie -o dist/dynamic_c.run dynamic_c.c

echo "./dist/dynamic_c.run"
