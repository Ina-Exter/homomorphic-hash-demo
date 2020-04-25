#include "shards.h"

int main(int argc, char *argv[]) {

	if (argc <= 1) {
		printf("Missing argument: filename\n");
		return 0;
	}

	// int beta = 131072;
	int beta = 512;
	int m = 128;
	int *nb_blocks;
	nb_blocks = malloc(sizeof(int));
	*nb_blocks = 0;
	mpz_t *blocks;
	read_blocks_from_file(argv[1], beta, m, nb_blocks, &blocks);

	free_blocks(*nb_blocks, m, blocks);
	
	free(nb_blocks);

	return 0;

}
