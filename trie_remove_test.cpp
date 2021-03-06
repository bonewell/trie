#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "trie.h"

struct TestingTrie : public Trie {
public:
  using Node::children;
  using Node::operator[];
};

using ::testing::Contains;
using ::testing::Eq;
using ::testing::Key;
using ::testing::Not;

TEST(TrieRemoveTest, RemoveElementFromSignleElementTree) {
  TestingTrie tree;
  tree["One"] = Node{true};

  tree.Remove("One");

  EXPECT_THAT(tree.children->empty(), Eq(true));
}

TEST(TrieRemoveTest, RemoveElementWithoutChildrenFromFirstLevel) {
  TestingTrie tree;
  tree["One"] = Node{true};
  tree["Two"] = Node{true};

  tree.Remove("One");

  EXPECT_THAT(*tree.children, Not(Contains(Key("One"))));
}

TEST(TrieRemoveTest, NoElementToRemoveFomFirstLevel) {
  TestingTrie tree;
  tree["On"] = Node{};
  tree["On"]["e"] = Node{true};
  tree["On"]["ce"] = Node{true};

  tree.Remove("On");

  EXPECT_THAT(*tree.children, Contains(Key("On")));
}

TEST(TrieRemoveTest, UnmarkRootElementWithChildren) {
  TestingTrie tree;
  tree["Deep"] = Node{true};
  tree["Deep"]["water"] = Node{true};
  tree["Deep"]["sea"] = Node{true};

  tree.Remove("Deep");

  ASSERT_THAT(*tree.children, Contains(Key("Deep")));
  EXPECT_THAT(tree["Deep"].marker, Eq(false));
}

TEST(TrieRemoveTest, MoveUpChildElementIfGrandParentIsMarked) {
  TestingTrie tree;
  tree["Deep"] = Node{true};
  tree["Deep"]["er"] = Node{true};
  tree["Deep"]["er"]["sea"] = Node{true};

  tree.Remove("Deeper");

  ASSERT_THAT(*tree["Deep"].children, Not(Contains(Key("er"))));
  EXPECT_THAT(*tree["Deep"].children, Contains(Key("ersea")));
}

TEST(TrieRemoveTest, RemoveParentAndCompressWithSingleChild) {
  TestingTrie tree;
  tree["Deep"] = Node{true};
  tree["Deep"]["sea"] = Node{true};

  tree.Remove("Deep");

  EXPECT_THAT(*tree.children, Contains(Key("Deepsea")));
  EXPECT_THAT(*tree.children, Not(Contains(Key("Deep"))));
}

TEST(TrieRemoveTest, RemoveElementFromSecondLevel) {
  TestingTrie tree;
  tree["On"] = Node{};
  tree["On"]["e"] = Node{true};
  tree["On"]["ce"] = Node{true};
  tree["On"]["ion"] = Node{true};

  tree.Remove("Once");

  EXPECT_THAT(*tree["On"].children, Not(Contains(Key("ce"))));
}

TEST(TrieRemoveTest, RemoveChildAndCompressWithParentLastChild) {
  TestingTrie tree;
  tree["On"] = Node{};
  tree["On"]["e"] = Node{true};
  tree["On"]["ce"] = Node{true};

  tree.Remove("One");

  EXPECT_THAT(*tree.children, Not(Contains(Key("On"))));
  EXPECT_THAT(*tree.children, Contains(Key("Once")));
}
