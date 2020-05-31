#ifndef HASHES_H
#define HASHES_H

#include <gmp.h>
#include <inttypes.h>
#include <stdbool.h>
#include "shards.h"

void compute_block_hash(mpz_t result, mpz_t p, uint32_t m, mpz_t g[m], mpz_t b[m]);

void generate_g(uint32_t m, mpz_t g[m], mpz_t p, uint32_t seed);

void generate_g_with_pq_exp(uint32_t m, mpz_t g[m], mpz_t p, mpz_t q, uint32_t seed);

void clear_mpz_vector(uint32_t size, mpz_t *vector);

void compute_file_hash(mpz_t **h_f, mpz_t p, uint32_t n, uint32_t m, mpz_t g[m], mpz_t *f);

bool check_auxblock_hash(mpz_t result, mpz_t p, uint32_t m, mpz_t g[m], mpz_t *h_f, auxblock *auxb);

#endif // HASHES_H
