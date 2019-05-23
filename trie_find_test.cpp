#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "trie.h"

struct TestingTrie : public Trie {
public:
  using Node::operator[];
};

using ::testing::Eq;

TEST(TrieFindTest, FindElementOnFirstLevel) {
  TestingTrie tree;
  tree["One"] = Node{true};

  EXPECT_THAT(tree.Find("One").first, Eq(true));
}

TEST(TrieFindTest, NoElementOnFirstLevel) {
  TestingTrie tree;
  tree["On"] = Node{};
  tree["On"]["e"] = Node{true};
  tree["On"]["ce"] = Node{true};

  EXPECT_THAT(tree.Find("On").first, Eq(false));
}

TEST(TrieFindTest, FindElementOnSecondLevel) {
  TestingTrie tree;
  tree["On"] = Node{};
  tree["On"]["e"] = Node{true};
  tree["On"]["ce"] = Node{true};

  EXPECT_THAT(tree.Find("Once").first, Eq(true));
}

TEST(TrieFindTest, NoElementWithShortKey) {
  TestingTrie tree;
  tree["Deepwater"] = Node{true};

  EXPECT_THAT(tree.Find("Deep").first, Eq(false));
}

TEST(TrieFindTest, NoElementWithLongKey) {
  TestingTrie tree;
  tree["Deep"] = Node{true};

  EXPECT_THAT(tree.Find("Deepwater").first, Eq(false));
}
