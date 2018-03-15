all:	grammar
	./grammar html.dot
	dot -Tsvg html.dot > html.svg

grammar:	grammar.cpp
	g++ --std=c++17 -o grammar grammar.cpp
