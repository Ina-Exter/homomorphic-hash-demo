
phony: main.c
	gcc -o main main.c hashes.c -lgmp

clean: 
	rm main
