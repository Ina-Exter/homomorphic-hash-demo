#ifndef SHARDS_H
#define SHARDS_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <gmp.h>

void read_blocks_from_file(char *file_name, int beta, int m, int *nb_blocks, mpz_t **blocks);

unsigned long long int read_file_size(FILE *file_descriptor);

void free_blocks(int n, int m, mpz_t *matrix);

#endif // SHARDS_H
