#include <gtest/gtest.h>
#include "multimap.h"

class Multimap_OneKey_Test : public ::testing::Test {
protected:
  Multimap<int, int> multimap;
  void SetUp() override {
    multimap.Insert(1, 2);
  }
};

TEST_F(Multimap_OneKey_Test, TestBody) {
  std::vector<int> values = multimap.GetAll(1);  // Fetch all values for key 1
  EXPECT_EQ(values.size(), 1);  // Expect only one value
  EXPECT_EQ(values[0], 2);  // The value should be 2
}

class Multimap_MultipleKeys_Test : public ::testing::Test {
protected:
  Multimap<int, int> multimap;
  void SetUp() override {
    multimap.Insert(1, 2);
    multimap.Insert(2, 3);
  }
};

TEST_F(Multimap_MultipleKeys_Test, TestBody) {
  std::vector<int> values = multimap.GetAll(1);  // Fetch all values for key 1
  EXPECT_EQ(values.size(), 1);  // Expect only one value
  EXPECT_EQ(values[0], 2);  // The value should be 2

  values = multimap.GetAll(2);  // Fetch all values for key 2
  EXPECT_EQ(values.size(), 1);  // Expect only one value
  EXPECT_EQ(values[0], 3);  // The value should be 3
}

class Multimap_MultipleValues_Test : public ::testing::Test {
protected:
  Multimap<int, int> multimap;
  void SetUp() override {
    multimap.Insert(1, 1);
    multimap.Insert(1, 2);
    multimap.Insert(1, 3);
  }
};

TEST_F(Multimap_MultipleValues_Test, TestBody) {
  std::vector<int> values = multimap.GetAll(1);  // Fetch all values for key 1
  EXPECT_EQ(values.size(), 3);  // Expect three values
  EXPECT_EQ(values[0], 1);  // The first value should be 1
  EXPECT_EQ(values[1], 2);  // The second value should be 2
  EXPECT_EQ(values[2], 3);  // The third value should be 3
}
