
CFLAGS = -Wall
LDFLAGS = -lm 
THREADS = -pthread
CC = gcc 
PROG = ex3_q1

all: test 

ex3_q1_given.o: ex3_q1_given.c ex3_q1_given.h
	$(CC)  $(CFLAGS) -c ex3_q1_given.c $(THREADS)
		
$(PROG): $(PROG).c $(PROG).h ex3_q1_given.o
	$(CC) -o $(PROG) ex3_q1_given.o $(PROG).c $(PROG).h $(LDFLAGS) $(THREADS)

test: $(PROG)  
	./$(PROG) > out.log 2> err.log

clean:
	rm -vf *.o 
	rm -vf *.log
	rm  $(PROG)