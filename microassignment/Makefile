DEBUG = -g
CPPFLAGS = -std=c++11 -pedantic -Wall -Wno-unused
CXXFLAGS = $(DEBUG)
SUBMIT_FILE = micro_project.tar.gz

.PHONY: part1 part2 part3 clean

all: part1 part2 part3

part1: example1

part2: example2

part3: example3

clean:
	rm -f example1 example2 example3

submit:
	rm -f $(SUBMIT_FILE)
	tar czvf $(SUBMIT_FILE) example1.cpp example2.cpp example3.cpp micro-project.txt
