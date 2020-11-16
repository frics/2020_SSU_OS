echo "Running test_alloc.c"
gcc test_alloc.c alloc.c -o test_alloc
./test_alloc
echo
echo "---------------------"
echo "Running test_ealloc.c"
gcc test_ealloc.c ealloc.c -o test_ealloc
./test_ealloc
