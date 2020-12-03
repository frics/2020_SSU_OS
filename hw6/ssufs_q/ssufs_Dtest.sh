]
echo "RUN OTHER TEST PROGRAM"

gcc -o ssufs_Dtest ssufs_Dtest.c ssufs-ops.c ssufs-disk.c
./ssufs_Dtest

rm -f ssufs
rm -f ssufs_test

