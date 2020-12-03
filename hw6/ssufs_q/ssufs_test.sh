#!/bin/bash

gcc -o ssufs_test ssufs_test.c ssufs-ops.c ssufs-disk.c
./ssufs_test

rm -f ssufs
rm -f ssufs_test

#echo "RUN OTHER TEST PROGRAM"
#echo
#echo
#gcc -o ssufs_Dtest ssufs_Dtest.c ssufs-ops.c ssufs-disk.c
#./ssufs_Dtest

#rm -f ssufs
#rm -f ssufs_test

