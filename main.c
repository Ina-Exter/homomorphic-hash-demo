#include <stdio.h>
#include "hashes.h"

int main(int argc, char *argv[]){

	//Constants
	int beta = 16384;
	int n = 32;
	int m = 512;

	//GMP values
	mpz_t p, q;
	mpz_init_set_str(p, "102686731697491850601835267854253299337604299200178657837019678284498057826446827698255312174665326750595850150279076305312627736388398129164524079191541447008637186459099223081547163467030215150192858147281551243771467206916188971410580763349140387748346660217508610393653748747785882176295238742904842223617", 10);
	mpz_init_set_str(q, "132284103105450116340217709048019731335405657496185202598840040211064482894297", 10);
	
	// Generate a vector g
	mpz_t g[m];
	for (int i = 0; i < m; i++) {
		mpz_init(g[i]);
	}
	generate_g(m, g, p, n);

	//Curiosity: print g
	for (int i = 0; i < m; i++) {
		gmp_printf("g[%d] = %Zd\n", i, g[i]);
	}

	//Select a matrix f and here we go
	// For now, generating with GMP cause simpler to modulo-sum
	mpz_t f[n][m];
	unsigned long int sum;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			sum = 1 + i + j;
			mpz_init_set_ui(f[i][j], sum);
		}
	}

	// compute compound block 1+2
	mpz_t block_one_plus_two[m];
	mpz_t agg;
	mpz_init(agg);
	for (int i = 0; i < m; i++) {
		mpz_add(agg, f[0][i], f[1][i]);
		// Additions on Zq?
		mpz_mod(agg, agg, q);
		mpz_init_set(block_one_plus_two[i], agg);
	}

	/*printf("Sanity check:\n");
       	for (int i = 0; i < m; i++) {
		gmp_printf("f[0][%d] = %Zd, f[1][%d] = %Zd, sum[%d] = %Zd\n", i, f[0][i], i, f[1][i], i, block_one_plus_two[i]);
	}*/

	// Compute hash of block 1, hash of block 2, hash of block 1+2 and product of hashes 1&2
	mpz_t b1h, b2h, b1plus2h, b1htimesb2h;
	mpz_inits(b1h, b2h, b1plus2h, b1htimesb2h, NULL);

	printf("Compute block hases 1 and 2\n");
	compute_block_hash(b1h, p, q, m, g, f[0]);
	compute_block_hash(b2h, p, q, m, g, f[1]);
	printf("Compute block hash of block(1+2)\n");
	compute_block_hash(b1plus2h, p, q, m, g, block_one_plus_two);
	printf("Compute product of block hash 1 and block hash 2\n");
	mpz_addmul(b1htimesb2h, b1h, b2h);
	mpz_mod(b1htimesb2h, b1htimesb2h, p);

	// Print values?
	gmp_printf("Hash of sum of blocks = %Zd\n", b1plus2h);
	gmp_printf("Product of hashes of blocks =  %Zd\n", b1htimesb2h);

	// Assert equality
	int equal = mpz_cmp(b1plus2h, b1htimesb2h);
	printf("Equality check (0 if equal) : %d\n", equal);

	//Clear
	for (int i = 0; i < m; i++) {
		mpz_clear(g[i]);
	}
	mpz_clear(p);
	mpz_clear(q);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			mpz_clear(f[i][j]);
		}
	}

	mpz_clear(agg);
	for (int i = 0; i < m; i++) {
		mpz_clear(block_one_plus_two[i]);
	}

	mpz_clears(b1h, b2h, b1plus2h, b1htimesb2h, NULL);

	return 0;

	//LEGACY	
	/*if (argc < 2) {
		printf("%d\n", argc);
		return 1;
	}

	int a;
	a = read_blocks_from_file(argv[1], 16384, 512);	
	*/

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
	
	return 0;
	*/
}
