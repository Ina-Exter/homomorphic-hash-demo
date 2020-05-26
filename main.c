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

	//Compute hash of file f
	printf("Computing hash of file (LONG i.e. 300-500s for large files i.e. 32+ MB)\n");
	mpz_t *h_f;
	compute_file_hash(&h_f, p, *nb_blocks, m, g, f);
	
	//compute compound block 1+2 from f
	printf("Computing compound block\n");
	auxblock *cpb;
	uint32_t parts1[2] = {0, 1};
	compute_compound_block(&cpb, m, p, q, f, 2, parts1);

	//Decomposition of "hashes.c" check_auxblock_hash function
	
	//The following uncommented code mimics the following commented code:
	/*start = clock();
	bool rop = check_auxblock_hash(auxbhash, p, m, g, h_f, cpb);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for hash verification (both calculations) = %f\n", cpu_time_used);
	printf("Test returns %d\n", rop);*/

	mpz_t auxblockhash, verify, bfactor;
	uint32_t current_block;
	mpz_init(auxblockhash); 
	mpz_init_set_str(verify, "1", 10);
	mpz_init(bfactor);

	//Time the raw hash calculation
	printf("Calculating auxiliary block hash\n");
	start = clock();
	compute_block_hash(auxblockhash, p, m, g, cpb->block);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for auxblock hash computation = %f\n", cpu_time_used);


	mpz_t agg;
	mpz_init(agg);

	//Time the verification
	printf("Calculating auxblock verification hash based on hash of file\n");
	start = clock();
	for (uint32_t i = 0; i < cpb->degree; i++) {
		current_block = cpb->parts[i];
		gmp_printf("For i=%d, block number=%d, coeff is %Zd\n", i, current_block, cpb->coeffs[i]);
		mpz_set(bfactor, cpb->coeffs[i]);
		mpz_set(agg, h_f[current_block]);
		mpz_powm_sec(agg, agg, bfactor, p);
		mpz_mul(verify, verify, agg);
		mpz_mod(verify, verify, p);	
	}
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for verification hash computation = %f\n", cpu_time_used);

	mpz_clears(bfactor, agg, NULL);

	int comp = mpz_cmp(auxblockhash, verify);
	printf("Checking equality (0 if equal) %d\n", comp);

	printf("Checking with function (match results)\n");
	mpz_t auxbhash;
	mpz_init(auxbhash);	
	bool rop = check_auxblock_hash(auxbhash, p, m, g, h_f, cpb);
	printf("Equality through func: %d\n", rop);

	int cmpeq = mpz_cmp(auxblockhash, auxbhash);
	printf("Debug: check if both auxblockhashes are equal %d\n", cmpeq);
	mpz_clear(auxbhash);

	gmp_printf("Debug: verify_hash: %Zd\n", verify);

	gmp_printf("Debug: auxblockhash: %Zd\n", auxblockhash);

	// Clearing file values
	free_block(*nb_blocks, h_f);
	clear_compound_block(m, cpb);
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
