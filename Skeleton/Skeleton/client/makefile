# Build all: clean, compile, and link

all: clean compile link

# Linking Rule
link:
	@echo "Linking source files"
	g++ -pthread -o bin/StompProtocol bin/StompFrame.o bin/Channel.o bin/EncoderDecoder.o \
bin/event.o bin/InputStream.o bin/keyboardInput.o bin/ConnectionHandler.o bin/StompClient.o 


# Compile Rule
compile:
	@echo "Compile StompClient"
	g++ -pthread -g -Wall -Weffc++ -std=c++11 -c -o bin/StompClient.o src/StompClient.cpp -Iinclude
	@echo "Compile Channel"
	g++ -g -Wall -Weffc++ -std=c++11 -c -o bin/Channel.o src/Channel.cpp -Iinclude
	@echo "Compile EncoderDecoder"
	g++ -g -Wall -Weffc++ -std=c++11 -c -o bin/EncoderDecoder.o src/EncoderDecoder.cpp -Iinclude
	@echo "Compile event"
	g++ -g -Wall -Weffc++ -std=c++11 -c -o bin/event.o src/event.cpp -Iinclude
	@echo "Compile InputStream"
	g++ -g -Wall -Weffc++ -std=c++11 -c -o bin/InputStream.o src/InputStream.cpp -Iinclude
	@echo "Compile keyboard"
	g++ -g -Wall -Weffc++ -std=c++11 -c -o bin/keyboardInput.o src/keyboardInput.cpp -Iinclude
	@echo "Compile StompFrame"
	g++ -g -Wall -Weffc++ -std=c++11 -c -o bin/StompFrame.o src/StompFrame.cpp -Iinclude
	@echo "Compile StompProtocol"
	g++ -g -Wall -Weffc++ -std=c++11 -c -o bin/StompProtocol.o src/StompProtocol.cpp -Iinclude
	@echo "Compile ConnectionHandler"
	g++ -g -Wall -Weffc++ -std=c++11 -c -o bin/ConnectionHandler.o src/ConnectionHandler.cpp -Iinclude



clean:
	@echo "Cleaning bin files"
	rm -rf bin/*

cleanObjAndrum:
	@rm -f *.o
	@clear
	valgrind --leak-check=full --show-reachable=yes ./bin/main
