
main: main.c hashes.c shards.c
	gcc -std=c99 -Wall -Wextra -o main main.c hashes.c shards.c -lgmp -lm

homomorphytest: homomorphytest.c hashes.c
	gcc -std=c99 -o homomorphytest homomorphytest.c hashes.c shards.c -lgmp -lm

filetest: filetest.c shards.c
	gcc -std=c99 -o filetest filetest.c shards.c -lgmp -lm	

hashtime: hashtime.c hashes.c shards.c
	gcc -std=c99 -Wall -Wextra -o hashtime hashtime.c hashes.c shards.c -lgmp -lm

generate:
	python3 generate_p_from_q.py

clean: 
	rm main filetest homomorphytest
.PHONY: clean

all: main homomorphytest filetest hashtime
.PHONY: all

