CC = gcc

default:
	${CC} -c -Wall -Werror -fpic test_lib.c
	${CC} -shared -o libtest_lib.so test_lib.o
	rm test_lib.o