#include <gtest/gtest.h>
#include <algorithm>
#include <string>
#include <vector>
#include "multimap.h"

// Test empty multimap
TEST(Multimap, EmptyMultimap) {
  Multimap<int, std::string> multimap;
  EXPECT_EQ(multimap.Size(), 0);
  EXPECT_FALSE(multimap.Contains(1));
  EXPECT_THROW(multimap.Get(1), std::runtime_error);
}

// Test insertion and retrieval of a single key-value pair
TEST(Multimap, OneKey) {
  Multimap<int, int> multimap;
  multimap.Insert(2, 2);
  
  EXPECT_EQ(multimap.Size(), 1);
  EXPECT_TRUE(multimap.Contains(2));
  
  const auto& values = multimap.Get(2);
  EXPECT_EQ(values.size(), 1);
  EXPECT_EQ(values[0], 2);
}

// Test insertion and retrieval of multiple key-value pairs with different keys
TEST(Multimap, MultipleKeys) {
  Multimap<int, int> multimap;
  std::vector<int> keys{2, 18, 42, 43};
  
  for (auto i : keys) {
    multimap.Insert(i, i);
  }
  
  EXPECT_EQ(multimap.Size(), 4);
  
  std::random_shuffle(keys.begin(), keys.end());
  for (auto i : keys) {
    EXPECT_TRUE(multimap.Contains(i));
    const auto& values = multimap.Get(i);
    EXPECT_EQ(values.size(), 1);
    EXPECT_EQ(values[0], i);
  }
}

// Test insertion of multiple values for the same key
TEST(Multimap, MultipleValues) {
  Multimap<std::string, int> multimap;
  
  multimap.Insert("test", 1);
  multimap.Insert("test", 2);
  multimap.Insert("test", 3);
  
  EXPECT_EQ(multimap.Size(), 3);
  EXPECT_TRUE(multimap.Contains("test"));
  
  const auto& values = multimap.Get("test");
  EXPECT_EQ(values.size(), 3);
  EXPECT_EQ(values[0], 1);
  EXPECT_EQ(values[1], 2);
  EXPECT_EQ(values[2], 3);
}

// Test removal of a key with a single value
TEST(Multimap, RemoveSingleValue) {
  Multimap<int, std::string> multimap;
  
  multimap.Insert(1, "one");
  multimap.Insert(2, "two");
  
  EXPECT_EQ(multimap.Size(), 2);
  
  multimap.Remove(1);
  
  EXPECT_EQ(multimap.Size(), 1);
  EXPECT_FALSE(multimap.Contains(1));
  EXPECT_TRUE(multimap.Contains(2));
}

// Test removal of a single value from a key with multiple values
TEST(Multimap, RemoveMultipleValues) {
  Multimap<int, std::string> multimap;
  
  multimap.Insert(1, "one");
  multimap.Insert(1, "ONE");
  multimap.Insert(1, "One");
  
  EXPECT_EQ(multimap.Size(), 3);
  
  // Remove the first value ("one")
  multimap.Remove(1);
  
  EXPECT_EQ(multimap.Size(), 2);
  EXPECT_TRUE(multimap.Contains(1));
  
  const auto& values = multimap.Get(1);
  EXPECT_EQ(values.size(), 2);
  EXPECT_EQ(values[0], "ONE");
  EXPECT_EQ(values[1], "One");
  
  // Remove the second value ("ONE")
  multimap.Remove(1);
  
  EXPECT_EQ(multimap.Size(), 1);
  EXPECT_TRUE(multimap.Contains(1));
  
  const auto& values2 = multimap.Get(1);
  EXPECT_EQ(values2.size(), 1);
  EXPECT_EQ(values2[0], "One");
  
  // Remove the last value ("One")
  multimap.Remove(1);
  
  EXPECT_EQ(multimap.Size(), 0);
  EXPECT_FALSE(multimap.Contains(1));
}

// Test Min and Max
TEST(Multimap, MinMax) {
  Multimap<int, std::string> multimap;
  
  multimap.Insert(5, "five");
  multimap.Insert(1, "one");
  multimap.Insert(10, "ten");
  
  EXPECT_EQ(multimap.Min(), 1);
  EXPECT_EQ(multimap.Max(), 10);
}

// Test multimap with complex types
TEST(Multimap, ComplexTypes) {
  Multimap<std::string, std::vector<int>> multimap;
  
  std::vector<int> v1 = {1, 2, 3};
  std::vector<int> v2 = {4, 5, 6};
  
  multimap.Insert("data1", v1);
  multimap.Insert("data1", v2);
  
  EXPECT_EQ(multimap.Size(), 2);
  
  const auto& values = multimap.Get("data1");
  EXPECT_EQ(values.size(), 2);
  EXPECT_EQ(values[0], v1);
  EXPECT_EQ(values[1], v2);
}

// Main function to run the tests
int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}