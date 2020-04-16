#include "hashes.h"


void compute_block_hash(mpz_t result, mpz_t p, mpz_t q, int m, mpz_t g[m], mpz_t b[m]) {
	mpz_t hash, buff;
	mpz_init_set_str(hash, "1", 10);
	mpz_init(buff);

	for (int i = 0; i < m; i++) {
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
void generate_g(int m, mpz_t g[m], mpz_t p, int n) {
	// Consider that g[m] is initialized
	// Initialize state
	gmp_randstate_t state;
	gmp_randinit_default(state);
	gmp_randseed_ui(state, (unsigned long int)n);
	// Compute g[i]
	for (int i = 0; i < m; i++) {
		mpz_urandomm(g[i], state, p);
	}
	gmp_randclear(state);
}

void compute_g_with_r(mpz_t *result, mpz_t g0, int m, mpz_t r[m]) {
	
}


