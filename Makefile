# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -pthread

# Source files
SOURCES = chash.c hashdb.c rwlocks.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
EXECUTABLE = chash

# Default target
all: $(EXECUTABLE)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link object files into the executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE)

# Clean rule
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

