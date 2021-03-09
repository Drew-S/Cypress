build: src/lexer.cpp src/parser.cpp src/main.cpp src/token.cpp src/nodes/node.hpp
	g++ -Wall -g -std=c++17 -I./src src/nodes/node.cpp src/token.cpp src/lexer.cpp src/parser.cpp src/main.cpp -o main

clean:
	rm main
