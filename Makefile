
phony: main.c
	gcc -std=c99 -o main main.c hashes.c -lgmp -lm

clean: 
	rm main
