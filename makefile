# Targets & general dependencies
PROGRAM = sws
HEADERS = network.h scheduler.h sws.h 
OBJS = network.o sws.o SJF.o scheduler.o MLFQ.o RR.o
ADD_OBJS = 
FILES = test*.txt

# compilers, linkers, utilities, and flags
CC = gcc
CFLAGS = -Wall -g -w -pthread
COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(CFLAGS) -o $@ 

# implicit rule to build .o from .c files
%.o: %.c $(HEADERS)
	$(COMPILE) -c -o $@ $<


# explicit rules
all: sws

$(PROGRAM): $(OBJS) $(ADD_OBJS)
	$(LINK) $(OBJS) $(ADD_OBJS)

lib: sws_gold.o 
	 ar -r libxsws.a sws_gold.o

clean:
	rm -f *.o $(PROGRAM) $(FILES) 

runmlfq:
	./$(PROGRAM) 38080 mlfq 1 &
	make test

runmlfq8:
	./$(PROGRAM) 38080 mlfq 8 &
	make test

runmlfq16:
	./$(PROGRAM) 38080 mlfq 16 &
	make test
runrr:
	./$(PROGRAM) 38080 rr 1 &
	make test

runrr8:
	./$(PROGRAM) 38080 rr 8 &
	make test

runrr16:
	./$(PROGRAM) 38080 rr 16 &
	make test

runsjf:
	./$(PROGRAM) 38080 sjf 1 &
	make test
runsjf8:
	./$(PROGRAM) 38080 sjf 8 &
	make test
runsjf16:
	./$(PROGRAM) 38080 sjf 16 &
	make test

test:
	./hydra.py < test1.in > test1.txt
	./hydra.py < test2.in > test2.txt
	./hydra.py < test3.in > test3.txt
	./hydra.py < test4.in > test4.txt
	./hydra.py < test5.in > test5.txt
	./hydra.py < test6.in > test6.txt
	./hydra.py < test7.in > test7.txt
	./hydra.py < test8.in > test8.txt
	./hydra.py < test9.in > test9.txt
	./hydra.py < test10.in > test10.txt

zip:
	rm -f sws.zip
	zip sws.zip network.c network.h makefile
