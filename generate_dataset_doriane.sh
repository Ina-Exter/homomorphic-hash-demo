#!/bin/bash

# File

head -c 1M </dev/urandom > "testfile_1m"
head -c 32K </dev/urandom > "testfile_32k"
head -c 25K </dev/urandom > "testfile_25k"
head -c 16K </dev/urandom > "testfile_16k"

# Dir

mkdir data_doriane

for i in {0..9}
do

# Hash times

	./hashtime testfile_1m 262144 1024
	./hashtime testfile_1m 32768 128
	./hashtime testfile_1m 16384 64
	./hashtime testfile_1m 8192 32
	./hashtime testfile_1m 4096 16

	./hashtime testfile_32k 8192 32
	./hashtime testfile_32k 1024 4
	./hashtime testfile_32k 512 2
	./hashtime testfile_32k 256 1

	./hashtime testfile_16k 4096 16
	./hashtime testfile_16k 512 2
	./hashtime testfile_16k 256 1

# Auxblock times

	./auxblocktest_d4 testfile_1m 262144 1024
	./auxblocktest_d32 testfile_1m 32768 128
	./auxblocktest_d64 testfile_1m 16384 64
	./auxblocktest_d128 testfile_1m 8192 32
	./auxblocktest_d256 testfile_1m 4096 16

	./auxblocktest_d4 testfile_32k 8192 32
	./auxblocktest_d32 testfile_32k 1024 4
	./auxblocktest_d64 testfile_32k 512 2
	./auxblocktest_d128 testfile_32k 256 1

	./auxblocktest_d4 testfile_16k 4096 16
	./auxblocktest_d32 testfile_16k 512 2
	./auxblocktest_d64 testfile_16k 256 1

# Auxblock verify times

	./auxblockverifytime_d4 testfile_1m 262144 1024
	./auxblockverifytime_d32 testfile_1m 32768 128
	./auxblockverifytime_d64 testfile_1m 16384 64
	./auxblockverifytime_d128 testfile_1m 8192 32
	./auxblockverifytime_d256 testfile_1m 4096 16
	
	./auxblockverifytime_d4 testfile_32k 8192 32
	./auxblockverifytime_d32 testfile_32k 1024 4
	./auxblockverifytime_d64 testfile_32k 512 2
	./auxblockverifytime_d128 testfile_32k 256 1

	./auxblockverifytime_d4 testfile_16k 4096 16
	./auxblockverifytime_d32 testfile_16k 512 2
	./auxblockverifytime_d64 testfile_16k 256 1

done

mkdir data_hash_times
mv data_beta* data_hash_times/
mv data_hash_times/ data_doriane/

mkdir data_auxblock_times
mv data_auxblock_beta* data_auxblock_times/
mv data_auxblock_times/ data_doriane/

mkdir data_auxblock_verify_times
mv data_auxblock_verification_beta* data_auxblock_verify_times/
mv data_auxblock_verify_times/ data_doriane/

