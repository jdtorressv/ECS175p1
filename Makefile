all: p1

p1: main.o
	g++ main.o -o p1 -lglut -lGL 
main.o: main.cpp  
	g++ -c main.cpp -lglut -lGL 
clean:
	rm -rf *o p1
