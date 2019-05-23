#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "trie.h"

struct TestingTree : public Trie {
public:
  using Node::children;
  using Node::operator[];
};

using ::testing::Contains;
using ::testing::Eq;
using ::testing::Key;
using ::testing::SizeIs;

TEST(TrieInsertTest, InsertIntoEmptyTree) {
  TestingTree tree;

  tree.Insert("One");

  EXPECT_THAT(*tree.children, Contains(Key("One")));
}
TEST(TrieInsertTest, InsertTwoDifferentElements) {
  TestingTree tree;
  tree["One"] = Node{true};

  tree.Insert("Two");

  EXPECT_THAT(*tree.children, Contains(Key("Two")));
}

TEST(TrieInsertTest, InsertElementWithLongKey) {
  TestingTree tree;
  tree["In"] = Node{true};

  tree.Insert("Inside");

  EXPECT_THAT(*tree["In"].children, SizeIs(1));
  EXPECT_THAT(*tree["In"].children, Contains(Key("side")));
}

TEST(TrieInsertTest, InsertTwoSimilarElements) {
  TestingTree tree;
  tree["One"] = Node{true};

  tree.Insert("Once");

  ASSERT_THAT(*tree.children, Contains(Key("On")));
  EXPECT_THAT(*tree["On"].children, Contains(Key("e")));
  EXPECT_THAT(*tree["On"].children, Contains(Key("ce")));
}

TEST(TrieInsertTest, InsertElementWithData) {
  TestingTree tree;
  auto data = new Data{};

  tree.Insert("Data", data);

  ASSERT_THAT(*tree.children, Contains(Key("Data")));
  EXPECT_THAT(tree["Data"].data, Eq(data));
}

TEST(TrieInsertTest, ElementInsertionSetsTrueMarker) {
  TestingTree tree;

  tree.Insert("One");

  ASSERT_THAT(*tree.children, Contains(Key("One")));
  EXPECT_THAT(tree["One"].marker, Eq(true));
}

TEST(TrieInsertTest, ElementInsertionSetsFalseMarker) {
  TestingTree tree;
  tree["One"] = Node{true};

  tree.Insert("Once");

  ASSERT_THAT(*tree.children, Contains(Key("On")));
  EXPECT_THAT(tree["On"].marker, Eq(false));
}

TEST(TrieInsertTest, DeepInsertion) {
  TestingTree tree;
  tree["De"] = Node{};
  tree["De"]["ep"] = Node{true};
  tree["De"]["ad"] = Node{true};

  tree.Insert("Deepwater");

  EXPECT_THAT(*tree["De"]["ep"].children, Contains(Key("water")));
}

TEST(TrieInsertTest, DeepInsertionOfSimilarElement) {
  TestingTree tree;
  tree["De"] = Node{};
  tree["De"]["ep"] = Node{true};
  tree["De"]["ad"] = Node{true};

  tree.Insert("Death");

  ASSERT_THAT(*tree["De"].children, Contains(Key("a")));
  EXPECT_THAT(*tree["De"]["a"].children, Contains(Key("d")));
  EXPECT_THAT(*tree["De"]["a"].children, Contains(Key("th")));
}

TEST(TrieInsertTest, InsertionIsJustMark) {
  TestingTree tree;
  tree["On"] = Node{};
  tree["On"]["e"] = Node{true};
  tree["On"]["ce"] = Node{true};

  tree.Insert("On");

  EXPECT_THAT(tree["On"].marker, Eq(true));
}

TEST(TrieInsertTest, KeepChildrenForReplacedElement) {
  TestingTree tree;
  tree["On"] = Node{};
  tree["On"]["e"] = Node{true};
  tree["On"]["ce"] = Node{true};

  tree.Insert("On");

  EXPECT_THAT(*tree["On"].children, Contains(Key("e")));
  EXPECT_THAT(*tree["On"].children, Contains(Key("ce")));
}


TEST(TrieInsertTest, InsertElementWithShortKey) {
  TestingTree tree;
  tree["Deepwater"] = Node{true};

  tree.Insert("Deep");

  ASSERT_THAT(*tree.children, Contains(Key("Deep")));
  EXPECT_THAT(*tree["Deep"].children, SizeIs(1));
  EXPECT_THAT(*tree["Deep"].children, Contains(Key("water")));
}
