#!/bin/bash

#echo "Enter beta:"
#read -r beta
#echo "Enter m:"
#read -r m

filesize=1
while [ "$filesize" -ne 134217728 ]
do
	head -c "$filesize" </dev/urandom >testfile
	./main testfile $1 $2
	filesize=$(( filesize * 2 ))
done
