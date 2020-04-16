#!/bin/bash

echo "Enter lambda_p"
read -r lambda_p
echo "Enter lambda_q"
read -r lambda_q
echo "Enter m"
read -r m

p=$(openssl prime -generate -bits $lambda_p)
q=$(openssl prime -generate -bits $lambda_q)

agg=0
while [[ $agg -ne m ]]
do	
	g=$(shuf -i 1-$p -n 1)
	echo "$g"
	agg=$agg+1
done

echo "p= $p"
echo "q= $q"
