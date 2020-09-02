
auxblockverifytime: auxblockverifytime.c hashes.c shards.c
	gcc -std=c99 -Wall -Wextra -o auxblockverifytime auxblockverifytime.c hashes.c shards.c -lgmp -lm

homomorphytest: homomorphytest.c hashes.c
	gcc -std=c99 -o homomorphytest homomorphytest.c hashes.c shards.c -lgmp -lm

filetest: filetest.c shards.c
	gcc -std=c99 -o filetest filetest.c shards.c -lgmp -lm	

hashtime: hashtime.c hashes.c shards.c
	gcc -std=c99 -Wall -Wextra -o hashtime hashtime.c hashes.c shards.c -lgmp -lm

auxblocktest: auxblocktest.c hashes.c shards.c
	gcc -std=c99 -Wall -Wextra -o auxblocktest auxblocktest.c hashes.c shards.c -lgmp -lm

generate:
	python3 generate_p_from_q.py

separatedegree:
	gcc -std=c99 -Wall -Wextra -o auxblocktest_d4 auxblocktest_degree4.c hashes.c shards.c -lgmp -lm
	gcc -std=c99 -Wall -Wextra -o auxblockverifytime_d4 auxblockverifytime_degree4.c hashes.c shards.c -lgmp -lm
	gcc -std=c99 -Wall -Wextra -o auxblocktest_d32 auxblocktest_degree32.c hashes.c shards.c -lgmp -lm
	gcc -std=c99 -Wall -Wextra -o auxblockverifytime_d32 auxblockverifytime_degree32.c hashes.c shards.c -lgmp -lm
	gcc -std=c99 -Wall -Wextra -o auxblocktest_d64 auxblocktest_degree64.c hashes.c shards.c -lgmp -lm
	gcc -std=c99 -Wall -Wextra -o auxblockverifytime_d64 auxblockverifytime_degree64.c hashes.c shards.c -lgmp -lm
	gcc -std=c99 -Wall -Wextra -o auxblocktest_d128 auxblocktest_degree128.c hashes.c shards.c -lgmp -lm
	gcc -std=c99 -Wall -Wextra -o auxblockverifytime_d128 auxblockverifytime_degree128.c hashes.c shards.c -lgmp -lm
	gcc -std=c99 -Wall -Wextra -o auxblocktest_d256 auxblocktest_degree256.c hashes.c shards.c -lgmp -lm
	gcc -std=c99 -Wall -Wextra -o auxblockverifytime_d256 auxblockverifytime_degree256.c hashes.c shards.c -lgmp -lm

clean: 
	rm auxblockverifytime filetest homomorphytest hashtime auxblocktest
.PHONY: clean

all: auxblockverifytime homomorphytest filetest hashtime auxblocktest
.PHONY: all

