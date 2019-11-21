OBJS	= main.o err.o expresions.o parser.o queue.o scanner.o stack.o stack_ex.o symtable.o
SOURCE	= main.c err.c expresions.c parser.c queue.c scanner.c stack.c stack_ex.c symtable.c
HEADER	= main.h err.h expresions.h parser.h queue.h scanner.h stack.h stack_ex.h structs.h symtable.h
OUT	= proj
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c 

err.o: err.c
	$(CC) $(FLAGS) err.c 

expresions.o: expresions.c
	$(CC) $(FLAGS) expresions.c 

parser.o: parser.c
	$(CC) $(FLAGS) parser.c 

queue.o: queue.c
	$(CC) $(FLAGS) queue.c 

scanner.o: scanner.c
	$(CC) $(FLAGS) scanner.c 

stack.o: stack.c
	$(CC) $(FLAGS) stack.c 

stack_ex.o: stack_ex.c
	$(CC) $(FLAGS) stack_ex.c 

symtable.o: symtable.c
	$(CC) $(FLAGS) symtable.c


clean:
	rm -f $(OBJS) $(OUT)
