# A makefile associates names (aka targets) with usually complex
# terminal commands. 
#
# Each command (or command sequence) begins with a target, which can
# be any name that you choose, left justified. This is followed by, on
# the same line, a list of file names or other targets called
# dependencies. Then, in subsequent lines are the sequence of
# commands, each prefixed by a tab.

all: test
test: test.o scan.o parse.o analyze.o
	g++ -o test test.o scan.o parse.o analyze.o
scan.o: scan.cc globals.h
	g++ -c -w scan.cc

parse.o: parse.cc globals.h
	g++ -c -w parse.cc

analyze.o: analyze.cc globals.h
	g++ -c -w analyze.cc

codegen1.o: codegen1.cc globals.h
	g++ -c -w codegen1.cc

calculator.o: calculator.cc globals.h
	g++ -c -w calculator.cc
test.o: test.cc globals.h
	g++ -c -w test.cc
clean:
	rm -f *.o test
