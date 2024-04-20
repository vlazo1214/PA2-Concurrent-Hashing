# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -pthread

# Source files
SOURCES = chash.c hashdb.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
EXECUTABLE = chash

# Determine the platform
ifeq ($(OS),Windows_NT)
	# If on Windows, set appropriate commands and file extensions
	RM = del /q
	EXECUTABLE := $(EXECUTABLE).exe
else
	RM = rm -f
endif

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
	$(RM) $(EXECUTABLE) $(OBJECTS)