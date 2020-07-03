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
	double t2h, t2v, t3h, t3v, t5h, t5v, t10h, t10v, t20h, t20v, t30h, t30v, t50h, t50v, t100h, t100v;

	//printf("Beta=%d, m=%d\n", beta, m);

	//GMP values
	mpz_t p, q;
	// lambda_p = 1024, lambda_q = 257
	mpz_init_set_str(p, "102686731697491850601835267854253299337604299200178657837019678284498057826446827698255312174665326750595850150279076305312627736388398129164524079191541447008637186459099223081547163467030215150192858147281551243771467206916188971410580763349140387748346660217508610393653748747785882176295238742904842223617", 10);
	mpz_init_set_str(q, "132284103105450116340217709048019731335405657496185202598840040211064482894297", 10);
	

	// lambda_p = 4096, lambda_q = 1025
	// I'm not fucking around.
	//mpz_init_set_str(p, "748653853953445739187349182494495619612372350984731139189922664554331664230285842017877741034762915855531569615345663281480737874173667214819236917839096544456271714982428824486640720564498203679576250474808245061399026074612844028665322222761617389969871582922911752697002046566632433781871989936502261581273091127138667999783253007418433729667415341676012462291670609366209626465882174874648872203787893776151075278948666768573671352869185210299169331209614268307028006665029923709376771985676165162539119235017216532067707933429347021036227601925817209857092763808345659683652010608930872067815123524799285106895644064476941899420523752500236326637973218613672934556455436779330546842544392763244268965645422196128570814438231959159300809990508955493128156885846151634865159413797635250697060927862345787271185235233373665672652948408929352888034888692431824777982038911987744506789560716179283248921654001750496979840970678595293078899400617800238347720467019562932694166393196311282080022645309191070991382291154754690810832827913259732545558862280867904949356989024706928347139371366136908730183704248615515588395629064072043660094541736618019033879582349305620913903173986473017449640220098014623328314649429162075781916000257", 10);
	//mpz_init_set_str(q, "257729647948620019467975184381274355771246270842656298030783692319889884398298521863915397054872063721609240239713792002196852611293947717794712213494322120215507835321375720798710639477987782174113664210375063101894883361828994511655508077019894963602303703022767760180718951844897351519496758525382361484597", 10);

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
	generate_g_with_pq_exp(m, g, p, q, *nb_blocks);
	//generate_g(m, g, p, *nb_blocks);

	//Compute hash of file f
	printf("Computing hash of file (LONG i.e. 300-500s for large files i.e. 32+ MB)\n");
	mpz_t *h_f;
	compute_file_hash(&h_f, p, *nb_blocks, m, g, f);

	//Declare blocks
	auxblock *cpb2, *cpb3, *cpb5, *cpb10, *cpb20, *cpb30, *cpb50, *cpb100;

	//compute compound block 1+2 from f
	printf("Computing compound block\n");
	uint32_t parts1[2] = {0, 1};
	compute_compound_block(&cpb2, m, q, f, 2, parts1);

	//1+2+3
	printf("Computing compound block 1 + 2 + 3\n");
	uint32_t parts2[3] = {0, 1, 2};
	compute_compound_block(&cpb3, m, q, f, 3, parts2);

	//5 blocks
	printf("Computing compound block of 5 blocks\n");
	uint32_t parts3[5] = {1, 5, 8, 13, 20};
	compute_compound_block(&cpb5, m, q, f, 5, parts3);
	
	//10
	printf("Computing compound block of 10 blocks\n");
	uint32_t parts4[10] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55};
	compute_compound_block(&cpb10, m, q, f, 10, parts4);

	//20
	printf("Computing compound block of 20 blocks\n");
	uint32_t parts5[20] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38};
	compute_compound_block(&cpb20, m, q, f, 20, parts5);

	//30
	printf("Computing compound block of 30 blocks\n");
	uint32_t parts6[30] = {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 78, 81, 84, 87};
	compute_compound_block(&cpb30, m, q, f, 30, parts6);

	//50
	printf("Computing compound block of 50 blocks\n");
	uint32_t parts7[50] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99};
	compute_compound_block(&cpb50, m, q, f, 50, parts7);

	//100
	printf("Computing compound block of 100 blocks\n");
	uint32_t parts8[100] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99};
	compute_compound_block(&cpb100, m, q, f, 100, parts8);

	//Decomposition of "hashes.c" check_auxblock_hash function
	
	//The following uncommented code mimics the following commented code:
	/*start = clock();
	bool rop = check_auxblock_hash(auxbhash, p, m, g, h_f, cpb);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken for hash verification (both calculations) = %f\n", cpu_time_used);
	printf("Test returns %d\n", rop);*/

	mpz_t auxblockhash, verify, bfactor, agg;
	uint32_t current_block;
	mpz_init(auxblockhash); 
	mpz_init(agg);
	mpz_init_set_str(verify, "1", 10);
	mpz_init(bfactor);

	// FOR BLOCK 2

	//Time the raw hash calculation
	printf("Calculating auxiliary block hash\n");
	start = clock();
	compute_block_hash(auxblockhash, p, m, g, cpb2->block);
	end = clock();
	t2h = ((double) (end - start)) / CLOCKS_PER_SEC;

	//Time the verification
	printf("Calculating auxblock verification hash based on hash of file\n");
	start = clock();
	for (uint32_t i = 0; i < cpb2->degree; i++) {
		current_block = cpb2->parts[i];
		//gmp_printf("For i=%d, block number=%d, coeff is %Zd\n", i, current_block, cpb->coeffs[i]);
		mpz_set(bfactor, cpb2->coeffs[i]);
		mpz_set(agg, h_f[current_block]);
		mpz_powm_sec(agg, agg, bfactor, p);
		mpz_mul(verify, verify, agg);
		mpz_mod(verify, verify, p);	
	}
	end = clock();
	t2v = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Time taken for verification hash computation = %f\n", cpu_time_used);

	int comp = mpz_cmp(auxblockhash, verify);
	printf("Checking equality (0 if equal) %d\n", comp);

	//RESET
	mpz_set_str(verify, "1", 10);
	clear_compound_block(m, cpb2);

	// FOR BLOCK 3
	
	//Time the raw hash calculation
	printf("Calculating auxiliary block hash\n");
	start = clock();
	compute_block_hash(auxblockhash, p, m, g, cpb3->block);
	end = clock();
	t3h = ((double) (end - start)) / CLOCKS_PER_SEC;

	//Time the verification
	printf("Calculating auxblock verification hash based on hash of file\n");
	start = clock();
	for (uint32_t i = 0; i < cpb3->degree; i++) {
		current_block = cpb3->parts[i];
		//gmp_printf("For i=%d, block number=%d, coeff is %Zd\n", i, current_block, cpb->coeffs[i]);
		mpz_set(bfactor, cpb3->coeffs[i]);
		mpz_set(agg, h_f[current_block]);
		mpz_powm_sec(agg, agg, bfactor, p);
		mpz_mul(verify, verify, agg);
		mpz_mod(verify, verify, p);	
	}
	end = clock();
	t3v = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Time taken for verification hash computation = %f\n", cpu_time_used);

	comp = mpz_cmp(auxblockhash, verify);
	printf("Checking equality (0 if equal) %d\n", comp);

	//RESET
	mpz_set_str(verify, "1", 10);
	clear_compound_block(m, cpb3);
	
	// FOR BLOCK 5
	
	//Time the raw hash calculation
	printf("Calculating auxiliary block hash\n");
	start = clock();
	compute_block_hash(auxblockhash, p, m, g, cpb5->block);
	end = clock();
	t5h = ((double) (end - start)) / CLOCKS_PER_SEC;

	//Time the verification
	printf("Calculating auxblock verification hash based on hash of file\n");
	start = clock();
	for (uint32_t i = 0; i < cpb5->degree; i++) {
		current_block = cpb5->parts[i];
		//gmp_printf("For i=%d, block number=%d, coeff is %Zd\n", i, current_block, cpb->coeffs[i]);
		mpz_set(bfactor, cpb5->coeffs[i]);
		mpz_set(agg, h_f[current_block]);
		mpz_powm_sec(agg, agg, bfactor, p);
		mpz_mul(verify, verify, agg);
		mpz_mod(verify, verify, p);	
	}
	end = clock();
	t5v = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Time taken for verification hash computation = %f\n", cpu_time_used);

	comp = mpz_cmp(auxblockhash, verify);
	printf("Checking equality (0 if equal) %d\n", comp);

	//RESET
	mpz_set_str(verify, "1", 10);
	clear_compound_block(m, cpb5);
	
	// FOR BLOCK 10
	
	//Time the raw hash calculation
	printf("Calculating auxiliary block hash\n");
	start = clock();
	compute_block_hash(auxblockhash, p, m, g, cpb10->block);
	end = clock();
	t10h = ((double) (end - start)) / CLOCKS_PER_SEC;

	//Time the verification
	printf("Calculating auxblock verification hash based on hash of file\n");
	start = clock();
	for (uint32_t i = 0; i < cpb10->degree; i++) {
		current_block = cpb10->parts[i];
		//gmp_printf("For i=%d, block number=%d, coeff is %Zd\n", i, current_block, cpb->coeffs[i]);
		mpz_set(bfactor, cpb10->coeffs[i]);
		mpz_set(agg, h_f[current_block]);
		mpz_powm_sec(agg, agg, bfactor, p);
		mpz_mul(verify, verify, agg);
		mpz_mod(verify, verify, p);	
	}
	end = clock();
	t10v = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Time taken for verification hash computation = %f\n", cpu_time_used);

	comp = mpz_cmp(auxblockhash, verify);
	printf("Checking equality (0 if equal) %d\n", comp);

	//RESET
	mpz_set_str(verify, "1", 10);
	clear_compound_block(m, cpb10);
	
	// FOR BLOCK 20
	
	//Time the raw hash calculation
	printf("Calculating auxiliary block hash\n");
	start = clock();
	compute_block_hash(auxblockhash, p, m, g, cpb20->block);
	end = clock();
	t20h = ((double) (end - start)) / CLOCKS_PER_SEC;

	//Time the verification
	printf("Calculating auxblock verification hash based on hash of file\n");
	start = clock();
	for (uint32_t i = 0; i < cpb20->degree; i++) {
		current_block = cpb20->parts[i];
		//gmp_printf("For i=%d, block number=%d, coeff is %Zd\n", i, current_block, cpb->coeffs[i]);
		mpz_set(bfactor, cpb20->coeffs[i]);
		mpz_set(agg, h_f[current_block]);
		mpz_powm_sec(agg, agg, bfactor, p);
		mpz_mul(verify, verify, agg);
		mpz_mod(verify, verify, p);	
	}
	end = clock();
	t20v = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Time taken for verification hash computation = %f\n", cpu_time_used);

	comp = mpz_cmp(auxblockhash, verify);
	printf("Checking equality (0 if equal) %d\n", comp);

	//RESET
	mpz_set_str(verify, "1", 10);
	clear_compound_block(m, cpb20);
	
	// FOR BLOCK 30
	
	//Time the raw hash calculation
	printf("Calculating auxiliary block hash\n");
	start = clock();
	compute_block_hash(auxblockhash, p, m, g, cpb30->block);
	end = clock();
	t30h = ((double) (end - start)) / CLOCKS_PER_SEC;

	//Time the verification
	printf("Calculating auxblock verification hash based on hash of file\n");
	start = clock();
	for (uint32_t i = 0; i < cpb30->degree; i++) {
		current_block = cpb30->parts[i];
		//gmp_printf("For i=%d, block number=%d, coeff is %Zd\n", i, current_block, cpb->coeffs[i]);
		mpz_set(bfactor, cpb30->coeffs[i]);
		mpz_set(agg, h_f[current_block]);
		mpz_powm_sec(agg, agg, bfactor, p);
		mpz_mul(verify, verify, agg);
		mpz_mod(verify, verify, p);	
	}
	end = clock();
	t30v = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Time taken for verification hash computation = %f\n", cpu_time_used);

	comp = mpz_cmp(auxblockhash, verify);
	printf("Checking equality (0 if equal) %d\n", comp);

	//RESET
	mpz_set_str(verify, "1", 10);
	clear_compound_block(m, cpb30);
	
	// FOR BLOCK 50
	
	//Time the raw hash calculation
	printf("Calculating auxiliary block hash\n");
	start = clock();
	compute_block_hash(auxblockhash, p, m, g, cpb50->block);
	end = clock();
	t50h = ((double) (end - start)) / CLOCKS_PER_SEC;

	//Time the verification
	printf("Calculating auxblock verification hash based on hash of file\n");
	start = clock();
	for (uint32_t i = 0; i < cpb50->degree; i++) {
		current_block = cpb50->parts[i];
		//gmp_printf("For i=%d, block number=%d, coeff is %Zd\n", i, current_block, cpb->coeffs[i]);
		mpz_set(bfactor, cpb50->coeffs[i]);
		mpz_set(agg, h_f[current_block]);
		mpz_powm_sec(agg, agg, bfactor, p);
		mpz_mul(verify, verify, agg);
		mpz_mod(verify, verify, p);	
	}
	end = clock();
	t50v = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Time taken for verification hash computation = %f\n", cpu_time_used);

	comp = mpz_cmp(auxblockhash, verify);
	printf("Checking equality (0 if equal) %d\n", comp);

	//RESET
	mpz_set_str(verify, "1", 10);
	clear_compound_block(m, cpb50);
	
	// FOR BLOCK 100
	
	//Time the raw hash calculation
	printf("Calculating auxiliary block hash\n");
	start = clock();
	compute_block_hash(auxblockhash, p, m, g, cpb100->block);
	end = clock();
	t100h = ((double) (end - start)) / CLOCKS_PER_SEC;

	//Time the verification
	printf("Calculating auxblock verification hash based on hash of file\n");
	start = clock();
	for (uint32_t i = 0; i < cpb100->degree; i++) {
		current_block = cpb100->parts[i];
		//gmp_printf("For i=%d, block number=%d, coeff is %Zd\n", i, current_block, cpb->coeffs[i]);
		mpz_set(bfactor, cpb100->coeffs[i]);
		mpz_set(agg, h_f[current_block]);
		mpz_powm_sec(agg, agg, bfactor, p);
		mpz_mul(verify, verify, agg);
		mpz_mod(verify, verify, p);	
	}
	end = clock();
	t100v = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Time taken for verification hash computation = %f\n", cpu_time_used);

	comp = mpz_cmp(auxblockhash, verify);
	printf("Checking equality (0 if equal) %d\n", comp);

	// FOR THE ALLIANCE
	mpz_clears(bfactor, agg, NULL);
	clear_compound_block(m, cpb100);

	/*printf("Checking with function (match results)\n");
	mpz_t auxbhash;
	mpz_init(auxbhash);	
	bool rop = check_auxblock_hash(auxbhash, p, m, g, h_f, cpb);
	printf("Equality through func: %d\n", rop);

	int cmpeq = mpz_cmp(auxblockhash, auxbhash);
	printf("Debug: check if both auxblockhashes are equal %d\n", cmpeq);
	mpz_clear(auxbhash);

	gmp_printf("Debug: verify_hash: %Zd\n", verify);

	gmp_printf("Debug: auxblockhash: %Zd\n", auxblockhash);*/

	// Clearing verification values
	mpz_clears(verify, auxblockhash, NULL); 

	// Clearing file values
	free_block(*nb_blocks, h_f);
	free_blocks(*nb_blocks, m, f);
	free(nb_blocks);

	// Clearing cryptographic security values
	mpz_clear(p);
	mpz_clear(q);
	clear_mpz_vector(m, g);

	//Write data to file
	FILE *data_file;
	char *datafilename;
	datafilename = malloc(1000*sizeof(char));
	sprintf(datafilename, "data_auxblock_verification_beta_%u_m_%u.csv", beta, m);
	data_file=fopen(datafilename, "a");
	fprintf(data_file, "%d, %f, %f\n", 2, t2h, t2v);
	fprintf(data_file, "%d, %f, %f\n", 3, t3h, t3v);
	fprintf(data_file, "%d, %f, %f\n", 5, t5h, t5v);
	fprintf(data_file, "%d, %f, %f\n", 10, t10h, t10v);
	fprintf(data_file, "%d, %f, %f\n", 20, t20h, t20v);
	fprintf(data_file, "%d, %f, %f\n", 30, t30h, t30v);
	fprintf(data_file, "%d, %f, %f\n", 50, t50h, t50v);
	fprintf(data_file, "%d, %f, %f\n", 100, t100h, t100v);

	fclose(data_file);

	free(datafilename);


	return 0;
}
