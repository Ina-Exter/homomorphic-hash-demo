#include <stdio.h>
#include "hashes.h"
#include "shards.h"

int main(int argc, char *argv[]){
	
	if (argc < 2) {
		printf("%d\n", argc);
		return 1;
	}

	int a;
	a = read_blocks_from_file(argv[1], 16384, 512);	
		
	/*EXAMPLE
	int m;
	m = 2;
	mpz_t result, p, q;
	mpz_t b[m];
	mpz_t g[m];

	//Initialize all this shit with easy values
	printf("Variable initalization\n");
	mpz_init_set_str(result, "1", 10);

	// 2 | (3-1) so all ok
	mpz_init_set_str(p, "31", 10);
	mpz_init_set_str(q, "10", 10);
	
	// I hope this works
	printf("Arrays initialization\n");
	mpz_init_set_str(b[0], "2", 10);
	mpz_init_set_str(b[1], "2", 10);

	mpz_init_set_str(g[0], "3", 10);
	mpz_init_set_str(g[1], "4", 10);

	printf("Hash computation\n");
	compute_block_hash(result, p, q, m, g, b);

	gmp_printf("Value of hash %Zd\n", result);

	printf("Memory liberation\n");
	mpz_clears(result, p, q, b[0], b[1], g[0], g[1], NULL);
	*/
	return 0;

}
