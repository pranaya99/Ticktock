CXX = g++
CXXFLAGS = -Wall -Werror -std=c++11

all: test_multimap cfs_sched

test_multimap: test_multimap.cc
	$(CXX) $(CXXFLAGS) -o test_multimap test_multimap.cc

cfs_sched: cfs_sched.cc
	$(CXX) $(CXXFLAGS) -o cfs_sched cfs_sched.cc

clean:
	rm -f test_multimap cfs_sched
