# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -Iinclude

# Directories
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

# Target executable
TARGET = $(BIN_DIR)/stomp_client

# Default rule
all: clean compile link

# Cleaning up generated files
clean:
	rm -rf $(BIN_DIR)/*
	@echo "Cleaned up all object and binary files."

# Compiling source files into object files
compile: $(BIN_DIR) $(OBJS)
	@echo "Compilation completed."

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linking the executable
link: $(TARGET)
	@echo "Linking completed."

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)
	@echo "Created bin directory."
