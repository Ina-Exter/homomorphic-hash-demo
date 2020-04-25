#include "shards.h"

int read_blocks_from_file(char *file_name, int beta, int m) {
	unsigned long long int file_size; // Taking our precautions
	FILE *file_descriptor;

	file_descriptor = fopen(file_name, "r");
	if (file_descriptor == NULL) {
		printf("Error while reading file.");
		return 1;
	}
	file_size = read_file_size(file_descriptor);
	
	// File size
	printf("File size in bytes %llu\n", file_size);
	
	// Raw block size
	/* Legacy code if beta and m are given/fixed
	unsigned long long int raw_block_size;
	raw_block_size = (file_size/nb_blocks) + 1; // adding one as a precaution

	printf("Raw block size \"beta\": %llu\n", raw_block_size);

	//TEST: Block size to 16KB
	//raw_block_size = 16 * 8 * 1024;
	//RESULT: We indeed get m=512

	// To decide the number of fragments per block (aka "m"), calculate it and round up to next power of 2
	unsigned long long int fragment_number;
	fragment_number = raw_block_size/(lambda_q-1);
	printf("unrounded fragment size %llu\n", fragment_number);
	// "Bit twiddling hack"?
	// "It just works". Source: dude trust me.
	fragment_number --;
	fragment_number |= fragment_number >> 1;
	fragment_number |= fragment_number >> 2;
	fragment_number |= fragment_number >> 4;
	fragment_number |= fragment_number >> 8;
	fragment_number |= fragment_number >> 16;
	fragment_number |= fragment_number >> 32;
	fragment_number++;

	printf("Number of sub-blocks per block, called \"m\" in paper: %llu\n", fragment_number);
	
	END LEGACY*/

	// LET beta and m be fixed.
	// Let F be a file of size t, and n the amount of blocks so that each block is exactly beta in weight and has m sub-blocks
	// Each sub block contains exactly gamma=beta/m bits
	// There is n=ceiling(t/beta) blocks, Padding will have to be implemented
	
	// Should we assume that beta/m is an integer? Would simplify things a lot
	float subblock_size_float=((float)beta)/((float)m);
	printf("beta/m ratio = %.5f\n", subblock_size_float);
	assert(subblock_size_float==floor(subblock_size_float) && (subblock_size_float)==ceil(subblock_size_float));
	
	int subblock_size = (int)subblock_size_float;
	printf("Subblocks will each contain %d bytes of data\n",subblock_size); 
	// That's 32 minimum. We will need unsigned long long ints.

	//Compute n and prepare allocation for F
	int n = ceil((long double)file_size/(long double)beta);
	printf("Preparing for n=%d blocks\n", n);

	//Now, we fill the blocs "vertically"
	//Manual allocation, (2d) arrays are horrible to use
	mpz_t *file;
	file = malloc(n*m*sizeof(mpz_t));
	if (file == NULL) {
		printf("Error in malloc.\n");
		return 1;
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			mpz_init(file[i * m + j]);
		}
	}

	char *buffer; int result;
	buffer = malloc(beta*sizeof(char));
	char substring[m];
	mpz_t import;
	mpz_init(import);
	//Read from file. Big oof
	for (int i = 0; i < n; i++) {
		//Load beta data in memory
		printf("Reading %dth block of data\n", i);
		result = fread(buffer, 1, beta, file_descriptor);
		printf("Except %d bits, read %d bits\n", beta, result);
		printf("read:\n%s\n", buffer);
		if (result != beta) {
			if (i == n-1) {
				//Padding
				printf("Now padding last block with %d zeroes\n", beta - result);
				for (int k = -1; k < (beta - result); k++) {
					buffer[result + k] = '0';
				}

			} else {
				printf("File reading error for block %d\n", i);
			}
		}
		//Split accross m
		//printf("Padded buffer:\n%s\n", buffer);
		for (int j = 0; j < m; j++) {
			//Extract substring
			//printf("Substring %d extraction for block %d\n", j, i);
			for (int l = 0; l < subblock_size; l++) {
				//printf("%c\n", buffer[j*subblock_size+l]);
				substring[l]=buffer[j*subblock_size+l];
			}
			//printf("Extracted substring:\n%s\n", substring);
			// Import value
			mpz_import(import, subblock_size, 1, sizeof(substring[0]), 0, 0, substring);
			//gmp_printf("Value as int: %Zd\n", import);
			//Paste as integer value in file
			mpz_set(file[i * m + j], import);
		}
	}
	mpz_clear(import);
	free(buffer);

	//Tests
	printf("Block extract:\n");
	gmp_printf("%Zd %Zd %Zd \n", file[0], file[1], file[2]);

	char *export;
	export = calloc(1, subblock_size*sizeof(char));
	printf("Block extract (translated):\n");
	mpz_export(export, NULL, 1, subblock_size, 0, 0, file[0]);
	printf("%s ", export);
	mpz_export(export, NULL, 1, subblock_size, 0, 0, file[1]);
	printf("%s ", export);
	mpz_export(export, NULL, 1, subblock_size, 0, 0, file[2]);
	printf("%s\n", export);

	free(export);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			mpz_clear(file[i * m + j]);
		}
	}

	free(file);

	return 0;
}

unsigned long long int read_file_size(FILE *file_descriptor) {
	fseek(file_descriptor, 0, SEEK_END); 
	unsigned long long int size = ftell(file_descriptor); 
	rewind(file_descriptor);
	return size;
}

