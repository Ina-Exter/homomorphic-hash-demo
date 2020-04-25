
phony: main.c hashes.c
	gcc -std=c99 -o main main.c hashes.c -lgmp -lm

filetest: filetest.c shards.c
	gcc -std=c99 -o filetest filetest.c shards.c -lgmp -lm	

generate:
	python3 generate_p_from_q.py

clean: 
	rm main filetest
