#!/bin/bash

make clean
make all

# Hash times

./generate_dataset.sh 64 16
./generate_dataset.sh 1024 128
./generate_dataset.sh 2048 128
./generate_dataset.sh 32768 1024
./generate_dataset.sh 65536 512


mkdir data_hash_times
mv data_beta* data_hash_times/

# Auxblock times

head -c 10M </dev/urandom >"testfile_10m"

./auxblocktest testfile_10m 32768 16
./auxblocktest testfile_10m 32768 128
./auxblocktest testfile_10m 32768 1024
./auxblocktest testfile_10m 32768 2048
./auxblocktest testfile_10m 32768 4096

mkdir data_auxblock_times
mv data_auxblock_m* data_auxblock_times/

# Auxblock verify times

./auxblockverifytime testfile_10m 16384 256
./auxblockverifytime testfile_10m 16384 512
./auxblockverifytime testfile_10m 16384 1024
./auxblockverifytime testfile_10m 16384 2048
./auxblockverifytime testfile_10m 16384 4096

mkdir data_auxblock_verify_times
mv data_auxblock_verification_beta* data_auxblock_verify_times/

