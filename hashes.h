#ifndef HASHES_H
#define HASHES_H

#include <gmp.h>

void compute_block_hash(mpz_t result, mpz_t p, mpz_t q, int m, mpz_t g[m], mpz_t b[m]);

void generate_g(int m, mpz_t g[m], mpz_t p, int seed);

void compute_g_with_r(mpz_t *result, mpz_t g0, int m, mpz_t r[m]);

void clear_mpz_vector(int size, mpz_t *vector);

#endif // HASHES_H
