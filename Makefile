
phony: main.c
	gcc -std=c99 -o main main.c hashes.c shards.c -lgmp -lm

clean: 
	rm main
