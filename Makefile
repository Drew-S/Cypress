build: lexer.cpp parser.cpp main.cpp
	g++ -Wall -g -std=c++11 lexer.cpp parser.cpp main.cpp -o main

clean:
	rm main
