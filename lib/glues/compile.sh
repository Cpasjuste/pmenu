arm-none-linux-gnueabi-gcc -c glues.c -o glues.o -I/usr/local/pandora/usr/include
arm-none-linux-gnueabi-ar rcs libglues.a glues.o
