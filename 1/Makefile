all: global local levenshtein

global: global_alignment.c
	cc global_alignment.c -o global

local: local_alignment.c
	cc local_alignment.c -o local

levenshtein: levenshtein.c
	cc levenshtein.c -o levenshtein

clean:
	rm levenshtein local global
