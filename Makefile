
phony: main.c
	gcc -std=c99 -o main main.c hashes.c -lgmp -lm

generate:
	python3 generate_p_from_q.py

clean: 
	rm main
