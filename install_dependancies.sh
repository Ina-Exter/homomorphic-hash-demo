#!/bin/bash

echo "This script uses sudo to install some dependancies, and apt-get as packet manager."

# Getting dependancies
sudo apt-get update
sudo apt-get upgrade -y
sudo apt-get install m4 -y

# Getting GNU MP
wget https://gmplib.org/download/gmp/gmp-6.2.0.tar.xz
tar -xf gmp-6.2.0.tar.xz

# Installing GNU MP
cd gmp-6.2.0.tar.xz || exit
./configure
make check
make install

# Just in case.
sudo cp /usr/local/lib/libgmp.a /usr/lib/

# Done
echo "Installation script over. If nothing crashed, GNU MP is now installed and can be linked with GCC's -lgmp option."
