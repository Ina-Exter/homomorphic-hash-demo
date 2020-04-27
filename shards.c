#include "shards.h"

void read_blocks_from_file(mpz_t **blocks, uintptr_t *nb_blocks, char *file_name, uint32_t beta, uint32_t m) {
	uint64_t file_size; // Taking our precautions
	FILE *file_descriptor;

	file_descriptor = fopen(file_name, "r");
	if (file_descriptor == NULL) {
		printf("Error while reading file.");
		return;
	}
	file_size = read_file_size(file_descriptor);
	
	// File size
	printf("File size in bytes %lu\n", file_size);
	
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
	
	uint32_t subblock_size = (uint32_t)subblock_size_float;
	printf("Subblocks will each contain %d bytes of data\n", subblock_size); 

	//Compute n and prepare allocation for F
	uint32_t n = ceil((long double)file_size/(long double)beta);
	printf("Preparing for n=%d blocks\n", n);
	*nb_blocks = n;

	//Now, we fill the blocs "vertically"
	//Manual allocation, (2d) arrays are horrible to use
	mpz_t *file;
	file = malloc(n*m*sizeof(mpz_t));
	if (file == NULL) {
		printf("Error in malloc.\n");
		return;
	}

	for (uint32_t i = 0; i < n; i++) {
		for (uint32_t j = 0; j < m; j++) {
			mpz_init(file[i * m + j]);
		}
	}

	char *buffer; uint32_t result;
	buffer = calloc(1, beta*sizeof(char)+1);
	char substring[subblock_size];
	mpz_t import;
	mpz_init(import);
	//Read from file. Big oof
	for (uint32_t i = 0; i < n; i++) {
		//Load beta data in memory
		//printf("Reading %dth block of data\n", i);
		result = fread(buffer, 1, beta, file_descriptor);
		//printf("Expected %d bits, read %d bits\n", beta, result);
		//Prevent from reading too far
		buffer[result]='\0';
		//printf("read:\n%s\n", buffer);
		if (result != beta) {
			if (i == n-1) {
				//Remove EOF
				buffer[result - 1] = '0';
				//Padding
				//printf("Now padding last block with %d zeroes\n", beta - result);
				for (uint32_t k = 0; k <= (beta - result); k++) {
					buffer[result + k] = '0';
				}

			} else {
				printf("File reading error for block %d\n", i);
			}
		}
		//Split accross m
		//printf("Padded buffer:\n%s\n", buffer);
		for (uint32_t j = 0; j < m; j++) {
			//Extract substring
			//printf("Substring %d extraction for block %d\n", j, i);
			for (uint32_t l = 0; l < subblock_size; l++) {
				//printf("Char %d is:\n", l);
				//printf("%c\n", buffer[j*subblock_size+l]);
				substring[l]=buffer[j*subblock_size+l];
			}
			//printf("Extracted substring:\n%s\n", substring);
			// Import value
			mpz_import(import, subblock_size, 1, sizeof(substring[0]), 1, 0, substring);
			//gmp_printf("Value as int: %Zd\n", import);
			//Paste as integer value in file
			mpz_set(file[i * m + j], import);
		}
	}
	mpz_clear(import);
	free(buffer);

	//Close file
	fclose(file_descriptor);

	//Return
	*blocks = file;
}

uint64_t read_file_size(FILE *file_descriptor) {
	fseek(file_descriptor, 0, SEEK_END); 
	uint64_t size = ftell(file_descriptor); 
	rewind(file_descriptor);
	return size;
}

void free_blocks(uint32_t n, uint32_t m, mpz_t *matrix) {

	for (uint32_t i = 0; i < n; i++) {
		for (uint32_t j = 0; j < m; j++) {
			mpz_clear(matrix[i * m + j]);
		}
	}

	free(matrix);
}

void extract_block(mpz_t **block, uint32_t block_number, uint32_t m, mpz_t *matrix) {

	mpz_t *b;
	b = malloc(m*sizeof(mpz_t));
	if (b == NULL) {
		printf("Error in malloc.\n");
		return;
	}

	for (uint32_t i = 0; i < m; i++) {
		mpz_init_set(b[i], matrix[block_number * m + i]);
	}

	*block = b;

}

void free_block(uint32_t m, mpz_t *block) {

	for (uint32_t i = 0; i < m; i++) {
		mpz_clear(block[i]);
	}

	free(block);

}
