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

	//printf("File divided in n=%lu blocks\n", *nb_blocks);
	
	// Generate a vector g
	mpz_t g[m];
	// n is used as seed for instance
	generate_g(m, g, p, *nb_blocks);

	//time for full file hash
	//printf("Compute file hash\n");
	mpz_t *filehash;
	start = clock();
	compute_file_hash(&filehash, p, *nb_blocks, m, g, f);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Time taken for hash of full file = %f\n", cpu_time_used);
	//gmp_printf("Full file hash %Zd\n", filehash);

	// Clearing file values
	free_block(*nb_blocks, filehash);
	free_blocks(*nb_blocks, m, f);
	free(nb_blocks);

	// Clearing cryptographic security values
	mpz_clear(p);
	mpz_clear(q);
	clear_mpz_vector(m, g);

	//Write data to file
	FILE *test_file, *data_file;
	test_file = fopen(argv[1], "r");
	uint64_t test_file_size = read_file_size(test_file);
	fclose(test_file);
	printf("Time taken for hash of file size %" PRIu64" = %f\n", test_file_size, cpu_time_used);

	char *datafilename;
	datafilename = malloc(1000*sizeof(char));
	sprintf(datafilename, "data_beta_%u_m_%u.csv", beta, m);
	data_file=fopen(datafilename, "a");
	fprintf(data_file, "%" PRIu64", %f\n", test_file_size, cpu_time_used);
	fclose(data_file);

	free(datafilename);
	return 0;
}
