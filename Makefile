# Compiler
CC = gcc

# Source directory
SRC_DIR = src

# Compiler flags
CFLAGS = -Wall -Werror -g

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(SRC:.c=.o)

# Target executable
TARGET = cvc5-interpreter

# Default target
all: $(TARGET)

# Linking the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)