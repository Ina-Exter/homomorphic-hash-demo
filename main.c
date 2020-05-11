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
	// lambda_p = 1024, lambda_q = 257
	/*mpz_init_set_str(p, "102686731697491850601835267854253299337604299200178657837019678284498057826446827698255312174665326750595850150279076305312627736388398129164524079191541447008637186459099223081547163467030215150192858147281551243771467206916188971410580763349140387748346660217508610393653748747785882176295238742904842223617", 10);
	mpz_init_set_str(q, "132284103105450116340217709048019731335405657496185202598840040211064482894297", 10);
	*/

	// lambda_p = 4096, lambda_q = 1025
	// I'm not fucking around.
	mpz_init_set_str(p, "748653853953445739187349182494495619612372350984731139189922664554331664230285842017877741034762915855531569615345663281480737874173667214819236917839096544456271714982428824486640720564498203679576250474808245061399026074612844028665322222761617389969871582922911752697002046566632433781871989936502261581273091127138667999783253007418433729667415341676012462291670609366209626465882174874648872203787893776151075278948666768573671352869185210299169331209614268307028006665029923709376771985676165162539119235017216532067707933429347021036227601925817209857092763808345659683652010608930872067815123524799285106895644064476941899420523752500236326637973218613672934556455436779330546842544392763244268965645422196128570814438231959159300809990508955493128156885846151634865159413797635250697060927862345787271185235233373665672652948408929352888034888692431824777982038911987744506789560716179283248921654001750496979840970678595293078899400617800238347720467019562932694166393196311282080022645309191070991382291154754690810832827913259732545558862280867904949356989024706928347139371366136908730183704248615515588395629064072043660094541736618019033879582349305620913903173986473017449640220098014623328314649429162075781916000257", 10);
	mpz_init_set_str(q, "257729647948620019467975184381274355771246270842656298030783692319889884398298521863915397054872063721609240239713792002196852611293947717794712213494322120215507835321375720798710639477987782174113664210375063101894883361828994511655508077019894963602303703022767760180718951844897351519496758525382361484597", 10);

	// Read a file for a matrix f
	// f is a simple array. Each line has m values, so to access f[i][j], compute f[i * m + j]
	mpz_t *f;
	uint32_t *nb_blocks;
	nb_blocks = malloc(sizeof(uint32_t));
	*nb_blocks = 0;
	read_blocks_from_file(&f, nb_blocks, argv[1], beta, m);

	printf("File divided in n=%u blocks\n", *nb_blocks);
	assert(*nb_blocks>1);

	//Check the "problem"?
	int is_sum_isomorphic;
	uint32_t max_index = (*nb_blocks) * m;
	for (uint32_t i = 0; i < max_index; i++) {
		is_sum_isomorphic = mpz_cmp(q, f[i]);
		if (is_sum_isomorphic <= 0) {
			printf("WARNING WARNING WARNING. Security parameter q is not large enough to ensure that the sum behaves as an isomorphism. This may/will prevent homomorphy. Increase m, or decrease beta, or compute a larger (p, q) couple. Warning for index %d\n", i);
			break;
		}
	}

	// Generate a vector g
	mpz_t g[m];
	// n is used as seed for instance
	generate_g(m, g, p, *nb_blocks);

	// Extract block 1 and 2
	/*printf("Extracting blocks\n");
	mpz_t *block1, *block2;
	extract_block(&block1, 0, m, f);
	extract_block(&block2, 1, m, f);*/

	auxblock *cpb;


	//compute compound block 1+2 from f
	printf("Computing compound block 1 + 2 with struct\n");
	uint32_t parts1[2] = {0, 1};
	start = clock();
	compute_compound_block(&cpb, m, p, q, f, 2, parts1);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for compound block creation = %f\n", cpu_time_used);
	clear_compound_block(m, cpb);

	//1+2+3
	printf("Computing compound block 1 + 2 + 3\n");
	uint32_t parts2[3] = {0, 1, 2};
	start = clock();
	compute_compound_block(&cpb, m, p, q, f, 3, parts2);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for compound block creation = %f\n", cpu_time_used);
	clear_compound_block(m, cpb);

	//5 blocks
	printf("Computing compound block of 5 blocks\n");
	uint32_t parts3[5] = {1, 5, 8, 13, 20};
	start = clock();
	compute_compound_block(&cpb, m, p, q, f, 5, parts3);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for compound block creation = %f\n", cpu_time_used);
	clear_compound_block(m, cpb);
	
	//10?
	printf("Computing compound block of 10 blocks\n");
	uint32_t parts4[10] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55};
	start = clock();
	compute_compound_block(&cpb, m, p, q, f, 10, parts4);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for compound block creation = %f\n", cpu_time_used);
	clear_compound_block(m, cpb);

	// Compute hash of block 1, hash of block 2, hash of block 1+2 and product of hashes 1&2
	/*mpz_t b1h, b2h, b1plus2h, b1htimesb2h;
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
	printf("Equality check (0 if equal) : %d\n", equal);*/

	//Checking
	/*printf("Block extract for block1:\n");
	gmp_printf("%Zd\n%Zd\n%Zd\n", block1[0], block1[1], block1[m-1]);
	printf("Block extract for block2:\n");
	gmp_printf("%Zd\n%Zd\n%Zd\n", block2[0], block2[1], block2[m-1]);
	printf("Block extract for block 1+2 via struct:\n");
	gmp_printf("%Zd\n%Zd\n%Zd\n", block_one_plus_two->block[0], block_one_plus_two->block[1], block_one_plus_two->block[m-1]);*/

	// Clearing temporary values that serve to verify homomorphy
	//free_block(m, block1);
	//free_block(m, block2);
	//mpz_clears(b1h, b2h, b1plus2h, b1htimesb2h, NULL);
	
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
