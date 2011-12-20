cd ../../libASE
make dummy
ranlib lib/dummy/libASE.a
cd ../apps/dummy
make controller-SPSA-ffANN-test
make dummy-SPSA-ffANN-test
bin\dummy-SPSA-ffANN-test\dummy-SPSA-ffANN-test.exe

