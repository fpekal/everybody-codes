libeverybody-codes.a: everybody-codes.o
	ar rcs libeverybody-codes.a everybody-codes.o

everybody-codes.o:
	g++ -O2 -std=c++26 -o everybody-codes.o -c src/everybody-codes.cpp

install:
	mkdir -p ${out}/lib
	mkdir -p ${out}/include
	cp libeverybody-codes.a ${out}/lib/
	cp -r include/* ${out}/include
