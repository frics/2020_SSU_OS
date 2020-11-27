echo "---------------------"
echo "Running test_alloc.c"
echo "---------------------"
gcc ./alloc/test_alloc.c ./alloc/alloc.c -o test_alloc
./test_alloc

echo "---------------------"
echo "Running test_dong.c"
echo "---------------------"
gcc ./alloc/test_dong.c ./alloc/alloc.c -o test_dong
./test_dong
echo
echo "---------------------"
echo "Running test_ealloc.c"
echo "---------------------"
gcc ./ealloc/test_ealloc.c ./ealloc/ealloc.c -o test_ealloc
./test_ealloc

rm test_alloc test_dong test_ealloc
