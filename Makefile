CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -pedantic
LDFLAGS = -lgtest -lgtest_main -pthread

TARGET = multimap_test
SRCS = test_multimap.cc

all: $(TARGET)

$(TARGET): $(SRCS) multimap.h
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

