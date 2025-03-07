#include <gtest/gtest.h>
#include <algorithm>
#include <vector>

#include "multimap.h"

// Test one key
TEST(Multimap, OneKey) {
  Multimap<int, int> multimap;
  std::vector<int> keys{2};

  for (auto i : keys) {
    multimap.Insert(i, i);
  }

  EXPECT_EQ(multimap.Contains(2), true);
  EXPECT_EQ(multimap.Get(2)[0], keys[0]);
}

// Test multiple keys
TEST(Multimap, MultipleKeys) {
  Multimap<int, int> multimap;
  std::vector<int> keys{2, 18, 42, 43};

  for (auto i : keys) {
    multimap.Insert(i, i);
  }

  std::random_shuffle(keys.begin(), keys.end());
  for (auto i : keys) {
    EXPECT_EQ(multimap.Contains(i), true);
    EXPECT_EQ(multimap.Get(i)[0], i);
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
