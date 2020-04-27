#include <stdio.h>
#include <time.h>
#include "hashes.h"
#include "shards.h"

int main(int argc, char *argv[]){

	if (argc <= 1) {
		printf("Missing argument: filename\n");
		return 0;
	}

	//Constants
	uint32_t beta = 1024;
	uint32_t m = 4;

	printf("Beta=%d, m=%d\n", beta, m);

	//GMP values
	mpz_t p, q;
	mpz_init_set_str(p, "102686731697491850601835267854253299337604299200178657837019678284498057826446827698255312174665326750595850150279076305312627736388398129164524079191541447008637186459099223081547163467030215150192858147281551243771467206916188971410580763349140387748346660217508610393653748747785882176295238742904842223617", 10);
	mpz_init_set_str(q, "132284103105450116340217709048019731335405657496185202598840040211064482894297", 10);
	
	// Read a file for a matrix f
	// f is a simple array. Each line has m values, so to access f[i][j], compute f[i * m + j]
	mpz_t *f;
	uintptr_t *nb_blocks;
	nb_blocks = malloc(sizeof(uint32_t));
	*nb_blocks = 0;
	read_blocks_from_file(&f, nb_blocks, argv[1], beta, m);

	printf("File divided in n=%lu blocks\n", *nb_blocks);
	
	if (*nb_blocks <= 1) {
		printf("Only 1 block available. Abort and change settings.\n");
		mpz_clears(p, q, NULL);
		free_blocks(*nb_blocks, m, f);
		free(nb_blocks);
		return 1;
	}

	// Generate a vector g
	mpz_t g[m];
	// n is used as seed for instance
	generate_g(m, g, p, *nb_blocks);

	// Extract block 1 and 2
	mpz_t *block1, *block2;
	extract_block(&block1, 0, m, f);
	extract_block(&block2, 1, m, f);

	// compute compound block 1+2 from f
	mpz_t block_one_plus_two[m];
	mpz_t agg;
	mpz_init(agg);
	for (uint32_t i = 0; i < m; i++) {
		mpz_add(agg, block1[i], block2[i]);
		mpz_mod(agg, agg, q);
		mpz_init_set(block_one_plus_two[i], agg);
	}
	mpz_clear(agg);

	// Compute hash of block 1, hash of block 2, hash of block 1+2 and product of hashes 1&2
	mpz_t b1h, b2h, b1plus2h, b1htimesb2h;
	mpz_inits(b1h, b2h, b1plus2h, b1htimesb2h, NULL);

	// Time
	clock_t start, end;
	double cpu_time_used;

	printf("Compute block hases 1 and 2\n");
	start = clock();
	compute_block_hash(b1h, p, q, m, g, block1);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for hash of b1 = %f\n", cpu_time_used);

	start = clock();
	compute_block_hash(b2h, p, q, m, g, block2);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for hash of b2 = %f\n", cpu_time_used);

	printf("Compute block hash of block(1+2)\n");
	start = clock();
	compute_block_hash(b1plus2h, p, q, m, g, block_one_plus_two);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for hash of b(1+2) = %f\n", cpu_time_used);
	
	printf("Compute product of block hash 1 and block hash 2\n");
	start = clock();
	mpz_addmul(b1htimesb2h, b1h, b2h);
	mpz_mod(b1htimesb2h, b1htimesb2h, p);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for product of hashes = %f\n", cpu_time_used);

	// Print values
	gmp_printf("Hash of sum of blocks = %Zd\n", b1plus2h);
	gmp_printf("Product of hashes of blocks =  %Zd\n", b1htimesb2h);

	// Assert equality
	int equal = mpz_cmp(b1plus2h, b1htimesb2h);
	printf("Equality check (0 if equal) : %d\n", equal);

	// Check blocks for errors?
	printf("Block extract for block1 in matrix:\n");
	gmp_printf("%Zd\n%Zd\n%Zd\n", f[0], f[1], f[m-1]);
	printf("Block extract for block1 extracted:\n");
	gmp_printf("%Zd\n%Zd\n%Zd\n", block1[0], block1[1], block1[m-1]);



	//time for full file hash
	printf("Compute file hash\n");
	mpz_t filehash;
	mpz_init(filehash);
	start = clock();
	compute_file_hash(filehash, p, q, *nb_blocks, m, g, f);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for hash of full file = %f\n", cpu_time_used);
	gmp_printf("Full file hash %Zd\n", filehash);
	mpz_clear(filehash);

	// Clearing temporary values that serve to verify homomorphy
	mpz_clears(b1h, b2h, b1plus2h, b1htimesb2h, NULL);
	for (uint32_t i = 0; i < m; i++) {
		mpz_clear(block_one_plus_two[i]);
	}
	free_block(m, block1);
	free_block(m, block2);

	// Clearing file values
	free_blocks(*nb_blocks, m, f);
	free(nb_blocks);

	// Clearing cryptographic security values
	mpz_clear(p);
	mpz_clear(q);
	clear_mpz_vector(m, g);

	return 0;
}
