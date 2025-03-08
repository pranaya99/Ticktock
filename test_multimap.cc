#include <gtest/gtest.h>  // C++ testing library header

#include <string>  // C++ system header
#include <vector>  // C++ system header
#include "multimap.h"
class Multimap_OneKey_Test : public ::testing::Test {
 protected:
  Multimap<int, std::string> mmap;
};

TEST_F(Multimap_OneKey_Test, InsertAndGet) {
  mmap.Insert(1, "value1");
  EXPECT_EQ(mmap.Get(1), "value1");
}

class Multimap_MultipleKeys_Test : public ::testing::Test {
 protected:
  Multimap<int, std::string> mmap;
};

TEST_F(Multimap_MultipleKeys_Test, InsertAndRetrieveMultipleKeys) {
  mmap.Insert(1, "value1");
  mmap.Insert(2, "value2");
  EXPECT_EQ(mmap.Get(1), "value1");
  EXPECT_EQ(mmap.Get(2), "value2");
}

class Multimap_MultipleValues_Test : public ::testing::Test {
 protected:
  Multimap<int, std::string> mmap;
};

TEST_F(Multimap_MultipleValues_Test, InsertMultipleValuesForSameKey) {
  mmap.Insert(1, "value1");
  mmap.Insert(1, "value2");
  std::vector<std::string> values = mmap.GetAll(1);
  EXPECT_EQ(values.size(), 2);
  EXPECT_EQ(values[0], "value1");
  EXPECT_EQ(values[1], "value2");
}
