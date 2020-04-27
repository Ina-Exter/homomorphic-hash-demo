#include "shards.h"

int main(int argc, char *argv[]) {

	if (argc <= 1) {
		printf("Missing argument: filename\n");
		return 0;
	}

	// int beta = 131072;
	uint32_t beta = 512;
	uint32_t m = 128;
	uint32_t subblock_size = beta/m;
	uintptr_t *nb_blocks;
	nb_blocks = malloc(sizeof(uint32_t));
	*nb_blocks = 0;
	mpz_t *blocks;
	read_blocks_from_file(&blocks, nb_blocks, argv[1], beta, m);

	printf("Block extract:\n");
	gmp_printf("%Zd %Zd %Zd \n", blocks[0], blocks[1], blocks[2]);

	char *export;
	export = malloc(subblock_size*sizeof(char) + 1);
	printf("Block extract (translated):\n");
	mpz_export(export, NULL, 1, subblock_size, 1, 0, blocks[0]);
	export[subblock_size] = '\0';
	printf("%s ", export);
	mpz_export(export, NULL, 1, subblock_size, 1, 0, blocks[1]);
	printf("%s ", export);
	export[subblock_size] = '\0';
	mpz_export(export, NULL, 1, subblock_size, 1, 0, blocks[2]);
	printf("%s\n", export);
	export[subblock_size] = '\0';

	free(export);


	free_blocks(*nb_blocks, m, blocks);
	
	free(nb_blocks);

	return 0;

}
