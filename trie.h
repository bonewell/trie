#ifndef TRIE_H
#define TRIE_H

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

struct Data{};  // just to show that we can keep some data in tree
struct Node;
using Children = std::unordered_map<std::string, Node>;
using ChildrenPtr = std::shared_ptr<Children>;
using Result = std::pair<bool, const Data*>;

struct Node {
  bool marker{false};
  const Data* data{nullptr};
  ChildrenPtr children = std::make_shared<Children>();

  Node& operator[](const std::string& key);
};

struct Trie : protected Node {
public:
  /**
   * Inserts element with key and data
   * If key already exists, data will be replace by new one.
   */
  void Insert(std::string_view key, const Data* data = nullptr);
  void Remove(std::string_view key);
  Result Find(std::string_view key) const;
};

std::ostream& operator<<(std::ostream& out, const Trie& tree);

#endif  // TRIE_H
