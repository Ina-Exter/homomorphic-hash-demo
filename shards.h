#ifndef SHARDS_H
#define SHARDS_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

int read_blocks_from_file(char *file_name, int beta, int m);

unsigned long long int read_file_size(FILE *file_descriptor);

#endif // SHARDS_H
