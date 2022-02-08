#!/bin/bash
if [ $# -eq 0 ]
	then
    		cho "No arguments supplied"
    		it 0 
fi
if [ -f "$1" ]; then
    make clean all target=raspberry file=$1
    rm *.o
    exit 0
else
    echo "$1 does not exists."

fi
exit 1
