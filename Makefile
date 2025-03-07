CXX = g++
CXXFLAGS = -Wall -Werror -std=c++14

GTEST_FLAGS = -lgtest -lgtest_main -pthread

all: test_multimap cfs_sched

test_multimap: test_multimap.cc multimap.h
	$(CXX) $(CXXFLAGS) -o $@ $< $(GTEST_FLAGS)

cfs_sched: cfs_sched.cc multimap.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f test_multimap cfs_sched *.o