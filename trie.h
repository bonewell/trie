#ifndef TRIE_H
#define TRIE_H

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

struct Data{};  // just to show that we can keep some data in tree

struct Node;
using ChildrenList = std::unordered_map<std::string, Node>;
using Children = std::shared_ptr<ChildrenList>;

struct Node {
  bool marker{false};
  const Data* data{nullptr};
  Children children = std::make_shared<ChildrenList>();

  /**
   * Inserts element with key and data
   * If key already exists, data will be replace by new one.
   * @param key
   * @param data
   */
  void Insert(std::string key, const Data* data = nullptr);
  void Remove(std::string_view key);

  using Result = std::pair<bool, const Data*>;
  Result Find(std::string_view key);
  Node& operator[](const std::string& key);

private:
  Node& AddChild(const std::string& key, bool marker, const Data* data,
      Children children = std::make_shared<ChildrenList>());
  inline void MoveChild(const std::string& key, Node& node);
  inline void DeleteChild(const std::string& key);
  void MarkChild(const std::string& key, const Data* data);
  void UnmarkChild(const std::string& key);
  void RemoveElement(std::string_view key);
  void DeleteElement(const std::string& key);
  void Compress(const std::string& key);
};

using Trie = Node;

std::ostream& operator<<(std::ostream& out, const Trie& tree);

#endif  // TRIE_H
