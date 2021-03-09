build: lexer.cpp parser.cpp main.cpp token.cpp nodes/node.hpp
	g++ -Wall -g -std=c++17 -I. nodes/node.cpp token.cpp lexer.cpp parser.cpp main.cpp -o main

clean:
	rm main
