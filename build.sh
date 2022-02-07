#!/bin/bash
	echo $1
    make clean all target=raspberry file=$1
    rm *.o
    exit 0
exit 1
