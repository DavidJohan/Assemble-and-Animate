cd ../../libASE
make dummy
ranlib lib/dummy/libASE.a
cd ../apps/dummy
make controller-bpANN-test
make dummy-bpANN-test
bin\dummy-bpANN-test\dummy-bpANN-test.exe