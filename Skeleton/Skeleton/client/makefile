CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: bin StompWCIClient

StompWCIClient: bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/Channel.o bin/StompFrame.o bin/StompProtocol.o bin/keyboardInput.o bin/InputStream.o bin/EncoderDecoder.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/Channel.o bin/StompFrame.o bin/StompProtocol.o bin/keyboardInput.o bin/InputStream.o bin/EncoderDecoder.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/Channel.o: src/Channel.cpp
	g++ $(CFLAGS) -o bin/Channel.o src/Channel.cpp
	
bin/StompFrame.o: src/StompFrame.cpp
	g++ $(CFLAGS) -o bin/StompFrame.o src/StompFrame.cpp	

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/keyboardInput.o: src/keyboardInput.cpp
	g++ $(CFLAGS) -o bin/keyboardInput.o src/keyboardInput.cpp

bin/InputStream.o: src/InputStream.cpp
	g++ $(CFLAGS) -o bin/InputStream.o src/InputStream.cpp

bin/EncoderDecoder.o: src/EncoderDecoder.cpp
	g++ $(CFLAGS) -o bin/EncoderDecoder.o src/EncoderDecoder.cpp	

bin:
	mkdir -p bin

.PHONY: clean
clean:
	rm -f bin/*.o bin/StompWCIClient
