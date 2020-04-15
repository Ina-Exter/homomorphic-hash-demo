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

	//Compute n and prepare allocation for F
	int n = ceil((long double)file_size/(long double)beta);
	printf("Preparing for n=%d blocks\n", n);

	//Now, we fill the blocs "vertically"
	

	return 0;
}

unsigned long long int read_file_size(FILE *file_descriptor) {
	fseek(file_descriptor, 0, SEEK_END); 
	unsigned long long int size = ftell(file_descriptor); 
	rewind(file_descriptor);
	return size;
}
