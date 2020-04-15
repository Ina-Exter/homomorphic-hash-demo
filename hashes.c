#include "hashes.h"


void compute_block_hash(mpz_t result, mpz_t p, mpz_t q, int m, mpz_t g[m], mpz_t b[m]) {
	mpz_t hash, buff;
	mpz_init_set_str(hash, "1", 10);
	mpz_init(buff);

	for (int i = 0; i < m; i++) {
		// hash = hash * g[i] ^ bij mod p = hash * buff
		// buff contains g[i] ^ bij mod p
		mpz_powm_sec(buff, g[i], b[i], p);
		gmp_printf("Value of buff = %Zd\n", buff);
		// Aggregate in hash
		// TODO: Verify that I can actually multiply the variable and store in itself...
		mpz_mul(hash, hash, buff);
	}

	//copy hash into result
	mpz_set(result, hash);
	//free
	mpz_clear(hash);
	mpz_clear(buff);
}

void compute_g(mpz_t *result, mpz_t g0, int m, mpz_t r[m]) {
	
}


