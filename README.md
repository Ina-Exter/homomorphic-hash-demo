# Homomorphic Hash Demo

A Clang homomorphic hash demo by Ina-Exter (pseudonym for double-blind review).

## Dependancies

GMP, with compilator GCC. Math library may also be used, as well as standard libraries.

## Point

This program serves as a demonstrator for another hashing scheme which adds another propriety to the hashing function: homomorphism. For a given file F sliced into blocks ranging from b0 to bn, honomorphism of the hash function h() is translated as h(bi+bj)=h(bi)\*h(bj). This allows for direct verification of "auxiliary blocks", which are made of the sum of original blocks bi, with help of the hash of the original file F, described as h(F) = (h(b0), ... , h(bn)).
Let a be an auxiliary block so that a = b1 + b2. h(a) can then be verified by computing h(b1)\*h(b2), using the values given in h(F). 

## Setup

* make generate 
Generate big primes for the hashing function. Not yet bridged with the c code, you may/will have to edit the primes into the code manually

* make <executable name>
Compile a specific executable. Start it with /<excutable name>

* make all
Compiles all executables.

* make clean
Removes all executables.

## Executables

### Regarding security parameters

This program requires large primes p and q to be used for some calculations. In the present built, for homomorphytest, p is stored on 1024 bits, and q on 257. For other applications, p is stored on 4096 bits and q on 1025. Note that we want p and q primes, and q must divide p-1. A (homemade) python script is provided to generate valid p and q **candidates**. With some bad luck, they might not be prime. In each program is already a hard-coded value of p and q.

### homomorphytest

Parameters: none

Returns a quick test with a phony matrix f such as f[i][j] = 1 + i + j, computes h(b0), h(b1), their product, and h(b0+b1) and checks for equality.

### filetest

Parameters: filename

Reads the file passed as parameter with select values of beta and m, and prints an extract. Used to check file reading

### hashtime

Parameters: filename beta m

Reads the file passed as parameter using the given values of beta and m, then proceeds to hash the file and write the hashing time in data\_beta\_(value)\_m\_(value).csv

Please note that for larger files, hashing can be LONG (100MB can take up to 500 seconds for select values of beta and m). Expect time fluctuation based on values of beta and m.

### auxblocktest

Parameters: filename beta m

Pre: Make sure the selected files and values of beta and m allow for at least 100 blocks.

Reads the file passed as parameter using the given values of beta and m, then proceeds to compute auxiliary blocks made of 2, 3, 5, 10, 20, 30, 50, and 100 blocks, and writes the computation times in data\_auxblock\_m\_(value).csv. 

### auxblockverifytime

Parameters: filename beta m

Pre: Make sure the selected files and values of beta and m allow for at least 100 blocks.

Reads the file passed as parameter using the given values of beta and m, computes the file hash, then proceeds to compute auxiliary blocks made of 2, 3, 5, 10, 20, 30, 50, and 100 blocks. It will then compute the block hash, the verification hash, test equality between the two and write the computation times in data\_auxblock\verification\_beta\_(value)\_\_m\_(value).csv. 

## Example

### Generating a random file

A random file of size 1MB can be generated with: ```head -c 1M </dev/urandom > test1mb``` 

### Starting an executable

```./auxblocktest test1mb 8192 512```

## Warnings

For bigger files and extreme values of beta and m, the hashing operation may be long.

This application requires quite a bit of RAM, even if some optimisations were made that reduced the memory required but makes the calculations a slight bit longer.

For some values of beta and m, elements of the file matrix may be larger than q. This WILL result in a complete loss of homomorphy, as the sum is no longer isomorphic. In some executables, a warning will be displayed.

The code written here is by no means certified to be used in real cryptographic setting. This merely is a PoC. Chances are side-channel attacks are possible nearly everywhere. You have been warned.

There may be some light profanity in the comments in the code. I get hotblooded when I make mistakes.

## References

On-the-Fly Verification of Rateless Erasure Codes for Efficient Content Distribution (Maxwell N. Krohn, MIT & Michael J. Freedman, NYU)

Cooperative Security for Network Coding File Distribution (Christos Gkantsidis and Pablo Rodriguez Rodriguez, Microsoft Research)
