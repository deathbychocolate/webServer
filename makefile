# Targets & general dependencies
PROGRAM = sws
HEADERS = network.h scheduler.h sws.h 
OBJS = network.o sws.o SJF.o scheduler.o MLFQ.o RR.o
ADD_OBJS = 
FILES = test*.out tests/Out/test.*.txt

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

run: 
	make runmlfq 
	make runrr 
	make runsjf

runmlfq:
	./$(PROGRAM) 38080 mlfq 1 &
	make testm1 
	make runmlfq8

runmlfq8:
	./$(PROGRAM) 38080 mlfq 8 &
	make testm2 
	make runmlfq16

runmlfq16:
	./$(PROGRAM) 38080 mlfq 16 &
	make testm3 
	make runmlfq32

runmlfq32:
	./$(PROGRAM) 38080 mlfq 32 &
	make testm4 
	make runmlfq64

runmlfq64:
	./$(PROGRAM) 38080 mlfq 64 &
	make testm5

runrr:
	./$(PROGRAM) 38080 rr 1 &
	make testr1 
	make runrr8
runrr8:
	./$(PROGRAM) 38080 rr 8 &
	make testr2 
	make runrr16

runrr16:
	./$(PROGRAM) 38080 rr 16 &
	make testr3 
	make runrr32

runrr32:
	./$(PROGRAM) 38080 rr 32 &
	make testr4 
	make runrr64

runrr64:
	./$(PROGRAM) 38080 rr 64 &
	make testr5

runsjf:
	./$(PROGRAM) 38080 sjf 1 &
	make tests1 
	make runsjf8

runsjf8:
	./$(PROGRAM) 38080 sjf 8 &
	make tests2 
	make runsjf16

runsjf16:
	./$(PROGRAM) 38080 sjf 16 &
	make tests3 
	make runsjf32

runsjf32:
	./$(PROGRAM) 38080 sjf 32 &
	make tests4 
	make runsjf64

runsjf64:
	./$(PROGRAM) 38080 sjf 64 &
	make tests5

testr1:
	./hydra.py < tests/In/test1.in > tests/Out/test.1.R.txt &
	./hydra.py < tests/In/test2.in > tests/Out/test.2.R.txt &
	./hydra.py < tests/In/test3.in > tests/Out/test.3.R.txt 
	
testr2:
	
	./hydra.py < tests/In/test4.in > tests/Out/test.4.R.txt &
	./hydra.py < tests/In/test5.in > tests/Out/test.5.R.txt &
	./hydra.py < tests/In/test6.in > tests/Out/test.6.R.txt 

testr3:
	./hydra.py < tests/In/test7.in > tests/Out/test.7.R.txt &
	./hydra.py < tests/In/test8.in > tests/Out/test.8.R.txt

testr4:
	./hydra.py < tests/In/test9.in > tests/Out/test.9.R.txt

testr5:
	./hydra.py < tests/In/test10.in > tests/Out/test.10.R.txt

tests1:
	./hydra.py < tests/In/test1.in > tests/Out/test.1.S.txt &
	./hydra.py < tests/In/test2.in > tests/Out/test.2.S.txt

tests2:
	./hydra.py < tests/In/test3.in > tests/Out/test.3.S.txt &
	./hydra.py < tests/In/test4.in > tests/Out/test.4.S.txt &
	./hydra.py < tests/In/test5.in > tests/Out/test.5.S.txt &
	./hydra.py < tests/In/test6.in > tests/Out/test.6.S.txt &
	./hydra.py < tests/In/test7.in > tests/Out/test.7.S.txt

tests3:
	./hydra.py < tests/In/test8.in > tests/Out/test.8.S.txt

tests4:
	./hydra.py < tests/In/test9.in > tests/Out/test.9.S.txt

tests5:
	./hydra.py < tests/In/test10.in > tests/Out/test.10.S.txt

testm1:
	./hydra.py < tests/In/test1.in > tests/Out/test.1.M.txt &
	./hydra.py < tests/In/test2.in > tests/Out/test.2.M.txt

testm2:
	./hydra.py < tests/In/test3.in > tests/Out/test.3.M.txt &
	./hydra.py < tests/In/test4.in > tests/Out/test.4.M.txt &
	./hydra.py < tests/In/test5.in > tests/Out/test.5.M.txt &
	./hydra.py < tests/In/test6.in > tests/Out/test.6.M.txt &
	./hydra.py < tests/In/test7.in > tests/Out/test.7.M.txt

testm3:
	./hydra.py < tests/In/test8.in > tests/Out/test.8.M.txt

testm4:
	./hydra.py < tests/In/test9.in > tests/Out/test.9.M.txt

testm5:
	./hydra.py < tests/In/test10.in > tests/Out/test.10.M.txt

zip:
	rm -f sws.zip
	zip sws.zip network.c network.h makefile
