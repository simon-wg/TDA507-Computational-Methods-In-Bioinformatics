CC = cc
CFLAGS = -Wall -Wextra -O2
TARGETS = global local levenshtein

.PHONY: all clean

all: $(TARGETS)

global: global_alignment.c
	$(CC) $(CFLAGS) $< -o $@

local: local_alignment.c
	$(CC) $(CFLAGS) $< -o $@

levenshtein: levenshtein.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
