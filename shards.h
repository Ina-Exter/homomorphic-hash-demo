#ifndef SHARDS_H
#define SHARDS_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>
#include <gmp.h>

void read_blocks_from_file(mpz_t **blocks, uintptr_t *nb_blocks, char *file_name, uint32_t beta, uint32_t m);

uint64_t read_file_size(FILE *file_descriptor);

void free_blocks(uint32_t n, uint32_t m, mpz_t *matrix);

void extract_block(mpz_t **block, uint32_t block_number, uint32_t m, mpz_t *matrix);

void free_block(uint32_t m, mpz_t *block);

#endif // SHARDS_H
