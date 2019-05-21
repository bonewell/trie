#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "trie.h"

using ::testing::Eq;

TEST(TrieFindTest, FindElementOnFirstLevel) {
  Trie tree;
  tree["One"] = Node{true};

  EXPECT_THAT(tree.Find("One").first, Eq(true));
}

TEST(TrieFindTest, NoElementOnFirstLevel) {
  Trie tree;
  tree["On"] = Node{};
  tree["On"]["e"] = Node{true};
  tree["On"]["ce"] = Node{true};

  EXPECT_THAT(tree.Find("On").first, Eq(false));
}

TEST(TrieFindTest, FindElementOnSecondLevel) {
  Trie tree;
  tree["On"] = Node{};
  tree["On"]["e"] = Node{true};
  tree["On"]["ce"] = Node{true};

  EXPECT_THAT(tree.Find("Once").first, Eq(true));
}

TEST(TrieFindTest, NoElementWithShortKey) {
  Trie tree;
  tree["Deepwater"] = Node{true};

  EXPECT_THAT(tree.Find("Deep").first, Eq(false));
}

TEST(TrieFindTest, NoElementWithLongKey) {
  Trie tree;
  tree["Deep"] = Node{true};

  EXPECT_THAT(tree.Find("Deepwater").first, Eq(false));
}
