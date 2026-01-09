CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lm
TARGET = longest_chain
SRC = main.c

# Default target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Pattern rule to build and run with a data file
# Usage: make data_q1.txt
%.txt: $(TARGET)
	./$(TARGET) $@

# Clean build artifacts
clean:
	rm -f $(TARGET)

.PHONY: clean
