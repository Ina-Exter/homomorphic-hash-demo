#include "shards.h"

void read_blocks_from_file(mpz_t **blocks, uint32_t *nb_blocks, char *file_name, uint32_t beta, uint32_t m) {
	uint64_t file_size; // Taking our precautions
	FILE *file_descriptor;

	file_descriptor = fopen(file_name, "r");
	if (file_descriptor == NULL) {
		printf("Error while reading file.");
		return;
	}
	file_size = read_file_size(file_descriptor);
	
	// File size
	//printf("File size in bytes %lu\n", file_size);
	
	// LET beta and m be fixed.
	// Let F be a file of size t, and n the amount of blocks so that each block is exactly beta in weight and has m sub-blocks
	// Each sub block contains exactly gamma=beta/m bits
	// There is n=ceiling(t/beta) blocks, Padding will have to be implemented
	
	// Should we assume that beta/m is an integer? Would simplify things a lot
	float subblock_size_float=((float)beta)/((float)m);
	//printf("beta/m ratio = %.5f\n", subblock_size_float);
	assert(subblock_size_float==floor(subblock_size_float) && (subblock_size_float)==ceil(subblock_size_float));
	
	uint32_t subblock_size = (uint32_t)subblock_size_float;
	//printf("Subblocks will each contain %d bytes of data\n", subblock_size); 

	//Compute n and prepare allocation for F
	uint32_t n = ceil((long double)file_size/(long double)beta);
	//printf("Preparing for n=%d blocks\n", n);
	*nb_blocks = n;
	
	//Now, we fill the blocs "vertically"
	//Manual allocation, (2d) arrays are horrible to use
	mpz_t *file;
	file = malloc((size_t)n*m*sizeof(mpz_t));
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
	buffer = calloc(1, (beta + 1)*sizeof(char));
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
		buffer[result-1]='\0';
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

void compute_compound_block(auxblock **result, uint32_t m, mpz_t q, mpz_t *matrix, uint32_t degree, uint32_t parts[degree]) {


	//Malloc the struct
	auxblock *auxb = malloc(sizeof(auxblock) + degree*sizeof(uint32_t));
	if (result == NULL) {
		printf("Error in malloc.\n");
		return;
	}
	auxb->degree = degree;
	//Malloc the indexes of blocks
	for (uint32_t i = 0; i < degree; i++) { auxb->parts[i] = parts[i]; }

	//Malloc the block
	mpz_t *b = malloc(m*sizeof(mpz_t));
	if (b == NULL) {
		printf("Error in malloc.\n");
		return;
	}
	// Oh yeah jimmy do NOT FORGET TO INIT THE BLOCK YOU SHITBUCKET
	for (uint32_t i = 0; i < m; i++) { mpz_init(b[i]); }

	printf("All mallocs passed \n");
	//Compute
	mpz_t *extracted_block;
	for (uint32_t i = 0; i < degree; i++) {
		//printf("Now attempting to extract block %u\n", parts[i]);
		extract_block(&extracted_block, parts[i], m, matrix);
		//printf("Extracted block %u\n", parts[i]);
		for (uint32_t j = 0; j < m; j++) {
			//printf("Add b[%u] and extracted_block[%u]\n", j, j);
			mpz_add(b[j], b[j], extracted_block[j]);
		}
		//printf("Freeing block %u\n", parts[i]);
		free_block(m, extracted_block);
	}
	//Mod
	for (uint32_t i = 0; i < m; i++) {
		mpz_mod(b[i], b[i], q);
	}
	
	auxb->block = b;

	*result = auxb;


}

void clear_compound_block(uint32_t m, auxblock *auxb) {

	free_block(m, auxb->block);
	free(auxb);

}
