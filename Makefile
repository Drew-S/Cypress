sources = src/lexer.cpp src/token.cpp src/parser.cpp src/nodes/node.cpp
main = src/main.cpp
tests = tests/test_lexer.cpp tests/test.cpp

build: $(sources) $(main)
	g++ -Wall -g -std=c++17 -I./src $(sources) $(main) -o main

test: $(sources) $(tests)
	g++ -Wall -g -std=c++17 -I./src -I./tests -lcppunit $(sources) $(tests) -o test

clean:
	rm main test
