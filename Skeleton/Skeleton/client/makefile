# Compiler and flags
CXX = g++
CXXFLAGS = -g -Wall -Weffc++ -std=c++11 -Iinclude
LDFLAGS = -pthread

# Directories
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.o, $(SOURCES))

# Target executable
TARGET = $(BIN_DIR)/StompClient


# Build all: clean, compile, and link
all: clean $(TARGET)

# Linking Rule
$(TARGET): $(OBJECTS)
	@echo "Linking source files"
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile Rule
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean Rule
clean:
	@echo "Cleaning bin files"
	rm -rf $(BIN_DIR)/*

# Additional clean and run with Valgrind
cleanObjAndRun:
	@rm -f *.o
	@clear
	valgrind --leak-check=full --show-reachable=yes ./$(TARGET)
