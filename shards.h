#ifndef SHARDS_H
#define SHARDS_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>
#include <gmp.h>

typedef struct auxblock auxblock;

struct auxblock {

	uint32_t degree;
	mpz_t *coeffs;
	mpz_t *block;
	uint32_t parts[];
};

void read_blocks_from_file(mpz_t **blocks, uint32_t *nb_blocks, char *file_name, uint32_t beta, uint32_t m);

uint64_t read_file_size(FILE *file_descriptor);

void free_blocks(uint32_t n, uint32_t m, mpz_t *matrix);

void extract_block(mpz_t **block, uint32_t block_number, uint32_t m, mpz_t *matrix);

void free_block(uint32_t m, mpz_t *block);

void compute_compound_block(auxblock **result, uint32_t m, mpz_t q, mpz_t *matrix, uint32_t degree, uint32_t parts[degree]);

void clear_compound_block(uint32_t m, auxblock *auxb);

#endif // SHARDS_H
