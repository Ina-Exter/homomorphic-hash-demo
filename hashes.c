#include "hashes.h"


void compute_block_hash(mpz_t result, mpz_t p, mpz_t q, uint32_t m, mpz_t g[m], mpz_t b[m]) {
	mpz_t hash, buff;
	mpz_init_set_str(hash, "1", 10);
	mpz_init(buff);

	for (uint32_t i = 0; i < m; i++) {
		// hash = hash * g[i] ^ bij mod p = hash * buff
		// buff contains g[i] ^ bij mod p
		mpz_powm_sec(buff, g[i], b[i], p);
		//gmp_printf("Value of buff = %Zd\n", buff);
		// Aggregate in hash
		mpz_mul(hash, hash, buff);
	}
	
	//Last modulo
	mpz_mod(hash, hash, p);
	//copy hash into result
	mpz_set(result, hash);
	//free
	mpz_clear(hash);
	mpz_clear(buff);
}



// n serves to seed the RN Generator
void generate_g(uint32_t m, mpz_t g[m], mpz_t p, uint32_t seed) {
	
	// Initialize g
	for (uint32_t i = 0; i < m; i++) {
		mpz_init(g[i]);
	}
	
	// Initialize state
	gmp_randstate_t state;
	gmp_randinit_default(state);
	gmp_randseed_ui(state, (unsigned long int)seed);
	
	// Compute g[i]
	for (uint32_t i = 0; i < m; i++) {
		mpz_urandomm(g[i], state, p);
	}
	gmp_randclear(state);
}

void clear_mpz_vector(uint32_t size, mpz_t *vector) {

	for (uint32_t i = 0; i < size; i++) { mpz_clear(vector[i]); }
}

void compute_file_hash(mpz_t result, mpz_t p, mpz_t q, uint32_t n, uint32_t m, mpz_t g[m], mpz_t *f) {

	//printf("Being file hash computation. This can take up to 360 seconds for 100MB...\n");
	mpz_t agg;
	mpz_init(agg);
	//Extract block 1
	mpz_t *block;
	extract_block(&block, 0, m, f);
	//Compute its hash
	compute_block_hash(agg, p, q, m, g, block); 
	//Free it...?
	free_block(m, block);
	mpz_t agg2;
	mpz_init(agg2);
	for (uint32_t i = 1; i < n; i++) {
		//printf("Handling block %d\n", i);
		extract_block(&block, i, m, f);
		compute_block_hash(agg2, p, q, m, g, block);
		free_block(m, block);
		//printf("Aggregating block %d\n", i);
		mpz_mul(agg, agg, agg2);
		mpz_mod(agg, agg, p);
	}
	mpz_clear(agg2);

	//last mod
	mpz_mod(agg, agg, p);

	mpz_set(result, agg);
	mpz_clear(agg);
}
