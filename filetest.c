#include "shards.h"

int main(int argc, char *argv[]) {

	if (argc <= 1) {
		printf("Missing argument: filename\n");
		return 0;
	}

	// beta 131072
	int a = read_blocks_from_file(argv[1], 512, 512);

	return 0;

}
