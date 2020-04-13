#ifndef HASHES_H
#define HASHES_H

#include <gmp.h>

// Define constants
#define BLOCK_SIZE 16000 //16 KB

// Base functions
void compute_block_hash(mpz_t result, mpz_t p, mpz_t q, int m, mpz_t g[m], mpz_t b[m]);
void compute_g(mpz_t *result, mpz_t g0, int m, mpz_t r[m]);

#endif // HASHES_H
