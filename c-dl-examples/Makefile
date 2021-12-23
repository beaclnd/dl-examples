CC = gcc

LD_LIBRARY_PATH = $(shell pwd)

dllinked:
	${CC} -c -Wall -Werror -fpic ../test_lib.c
	${CC} -shared -o libtest_lib.so test_lib.o
	${CC} -L${LD_LIBRARY_PATH} -Wall -D DLLINKED -o main main.c -ltest_lib
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH} ./main
	rm libtest_lib.so test_lib.o

dlopened:
	${CC} -c -Wall -Werror -fpic ../test_lib.c
	${CC} -shared -o libtest_lib.so test_lib.o
	${CC} -Wall -D DLOPENED -o main main.c -ldl
	./main
	rm libtest_lib.so test_lib.o
