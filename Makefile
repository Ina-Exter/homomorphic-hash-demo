
phony: main.c
	gcc -o main main.c -lgmp

clean: 
	rm main
