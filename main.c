#include <stdio.h>
#include <time.h>
#include "hashes.h"
#include "shards.h"

int main(int argc, char *argv[]){

	if (argc <= 3) {
		printf("Missing argument. Order: filename beta m\n");
		return 0;
	}

	//Constants
	//uint32_t beta = 1024;
	//uint32_t m = 64;
	uint32_t beta = (uint32_t)atoi(argv[2]);
	uint32_t m = (uint32_t)atoi(argv[3]);

	//Time
	clock_t start, end;
	double cpu_time_used;

	//printf("Beta=%d, m=%d\n", beta, m);

	//GMP values
	mpz_t p, q;
	mpz_init_set_str(p, "102686731697491850601835267854253299337604299200178657837019678284498057826446827698255312174665326750595850150279076305312627736388398129164524079191541447008637186459099223081547163467030215150192858147281551243771467206916188971410580763349140387748346660217508610393653748747785882176295238742904842223617", 10);
	mpz_init_set_str(q, "132284103105450116340217709048019731335405657496185202598840040211064482894297", 10);
	
	// Read a file for a matrix f
	// f is a simple array. Each line has m values, so to access f[i][j], compute f[i * m + j]
	mpz_t *f;
	uint32_t *nb_blocks;
	nb_blocks = malloc(sizeof(uint32_t));
	*nb_blocks = 0;
	read_blocks_from_file(&f, nb_blocks, argv[1], beta, m);

	printf("File divided in n=%u blocks\n", *nb_blocks);
	assert(*nb_blocks>1);

	// Generate a vector g
	mpz_t g[m];
	// n is used as seed for instance
	generate_g(m, g, p, *nb_blocks);

	// Extract block 1 and 2
	printf("Extracting blocks\n");
	mpz_t *block1, *block2;
	extract_block(&block1, 0, m, f);
	extract_block(&block2, 1, m, f);

	//compute compound block 1+2 from f
	printf("Computing compound block struct\n");
	auxblock *block_one_plus_two;
	uint32_t parts[2] = {1, 0};
	start = clock();
	compute_compound_block(&block_one_plus_two, m, q, f, 2, parts);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for compound block creation = %f\n", cpu_time_used);

	// Compute hash of block 1, hash of block 2, hash of block 1+2 and product of hashes 1&2
	mpz_t b1h, b2h, b1plus2h, b1htimesb2h;
	mpz_inits(b1h, b2h, b1plus2h, b1htimesb2h, NULL);

	printf("Compute block hases 1 and 2\n");
	compute_block_hash(b1h, p, q, m, g, block1);
	compute_block_hash(b2h, p, q, m, g, block2);

	printf("Compute block hash of block(1+2)\n");
	start = clock();
	compute_block_hash(b1plus2h, p, q, m, g, block_one_plus_two->block);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for hash of b(1+2) = %f\n", cpu_time_used);
	
	printf("Compute product of block hash 1 and block hash 2\n");
	mpz_addmul(b1htimesb2h, b1h, b2h);
	mpz_mod(b1htimesb2h, b1htimesb2h, p);

	// Print values
	gmp_printf("Hash of sum of blocks = %Zd\n", b1plus2h);
	gmp_printf("Product of hashes of blocks =  %Zd\n", b1htimesb2h);

	// Assert equality
	int equal = mpz_cmp(b1plus2h, b1htimesb2h);
	printf("Equality check (0 if equal) : %d\n", equal);

	//Checking
	printf("Block extract for block1:\n");
	gmp_printf("%Zd\n%Zd\n%Zd\n", block1[0], block1[1], block1[m-1]);
	printf("Block extract for block2:\n");
	gmp_printf("%Zd\n%Zd\n%Zd\n", block2[0], block2[1], block2[m-1]);
	printf("Block extract for block 1+2 via struct:\n");
	gmp_printf("%Zd\n%Zd\n%Zd\n", block_one_plus_two->block[0], block_one_plus_two->block[1], block_one_plus_two->block[m-1]);

	// Clearing temporary values that serve to verify homomorphy
	free_block(m, block1);
	free_block(m, block2);
	mpz_clears(b1h, b2h, b1plus2h, b1htimesb2h, NULL);
	clear_compound_block(m, block_one_plus_two);

	// Clearing file values
	free_blocks(*nb_blocks, m, f);
	free(nb_blocks);

	// Clearing cryptographic security values
	mpz_clear(p);
	mpz_clear(q);
	clear_mpz_vector(m, g);

	//Write data to file
	/*FILE *test_file, *data_file;
	test_file = fopen(argv[1], "r");
	uint64_t test_file_size = read_file_size(test_file);
	fclose(test_file);
	printf("Time taken for hash of file size %lu = %f\n", test_file_size, cpu_time_used);

	char *datafilename;
	datafilename = malloc(1000*sizeof(char));
	sprintf(datafilename, "data_beta_%u_m_%u.csv", beta, m);
	data_file=fopen(datafilename, "a");
	fprintf(data_file, "%lu, %f\n", test_file_size, cpu_time_used);

	free(datafilename);
	*/
	return 0;
}
