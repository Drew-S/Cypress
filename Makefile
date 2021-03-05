build: lexer.cpp parser.cpp main.cpp token.cpp
	g++ -Wall -g -std=c++17 -I. token.cpp lexer.cpp parser.cpp main.cpp -o main

clean:
	rm main
