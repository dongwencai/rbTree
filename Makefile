CC :=gcc -g -Wall
SRC :=$(wildcard *.c)
OBJ :=$(SRC:%.c=%.o)
main:$(OBJ)
	$(CC) -o $@ $^ -lpthread
$(OBJ):%.o:%.c
	$(CC) -c $^ -o $@ -lpthread

clean:
	rm -rf *.o main
