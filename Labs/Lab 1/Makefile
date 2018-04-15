message: message.o main.o
	g++ -o message message.o main.o

message.o main.o:
	g++ -c message.cpp
	g++ -c main.cpp

clean:
	rm message.o main.o message