all: test_multimap

test_multimap: test_multimap.o
	g++ -std=c++11 -o test_multimap test_multimap.o -lgtest -lgtest_main -pthread

test_multimap.o: test_multimap.cc
	g++ -std=c++11 -c test_multimap.cc

clean:
	rm -f test_multimap test_multimap.o
