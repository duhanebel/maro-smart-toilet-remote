CC=g++
CFLAGS=-c -Wall -I../src
EXECUTABLE=tests

src = $(wildcard *.cpp) $(filter-out ../src/main.cpp, $(wildcard ../src/*.cpp))
obj = $(src:.cpp=.o)
dep = $(obj:.o=.d)  # one dependency file for each source

all: clean cleandep $(EXECUTABLE)

$(EXECUTABLE): $(obj) 
	$(CC) -o $@ $^ 

-include $(dep)   # include all dep files in the makefile

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

test: clean cleandep $(EXECUTABLE)
	./$(EXECUTABLE)

.PHONY: clean
clean:
	rm -f $(obj) $(EXECUTABLE) 

.PHONY: cleandep
cleandep:
	rm -f $(dep)