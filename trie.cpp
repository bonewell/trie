#include "trie.h"

#include <algorithm>
#include <utility>

namespace {
struct MatcherKey {
  std::string find_key;
  size_t pos;
  std::string key;

  operator bool() const {
    return pos > 0 && key == find_key;
  }
  bool operator !() const {
    return pos == 0;
  }

  bool Partial() const {
    return pos > 0 && key == find_key.substr(0, pos);
  }

  bool IsPrefix() const {
    return pos > 0 && key.substr(0, pos) == find_key;
  }

  std::string Prefix() const {
    return key.substr(0, pos);
  }

  std::string Sufix() const {
    return key.substr(pos);
  }

  std::string Tail() const {
    return find_key.substr(pos);
  }
};

/**
 * Compares two strings and return position where is different found
 * @param lhs the first string
 * @param rhs the second string
 * @return position where chars are not equal
 * if strings are absolutely different, position is 0
 * if strings are similar and one of them is just longer,
 * position is std::min(lhs.length(), rhs.length())
 *
 */
size_t Compare(std::string_view lhs, std::string_view rhs) {
  auto min = std::min(lhs.length(), rhs.length());
  for (auto i = 0; i < min; ++i) {
    if (lhs[i] != rhs[i]) return i;
  }
  return min;
}

MatcherKey FindSimilarKey(ChildrenPtr children, std::string_view key) {
  for (const auto& c: *children) {
    auto pos = Compare(c.first, key);
    if (pos > 0) {
      return {std::string{key}, pos, c.first};
      // It is not posible to have few similar keys on the same level
    }
  }
  return {};
}

std::ostream& PrintChildren(std::ostream& out , const Node& node, int deep) {
  std::string prefix(deep, '-');
  for (const auto& c: *node.children) {
    out << prefix << c.first << "\n";
    PrintChildren(out, c.second, deep + 1);
  }
  return out;
}

inline void MarkNode(Node& node, const Data* data) {
  node.marker = true;
  node.data = data;
}

inline void UnmarkNode(Node& node) {
  node.marker = false;
  node.data = nullptr;
}

/**
 * Compress a child of the node
 */
void CompressNode(Node& parent, const std::string& key) {
  auto& node = parent.children->at(key);
  if (node.children->size() == 1) {
    const auto& child_key = node.children->begin()->first;
    const auto& node_child = node.children->begin()->second;
    parent.children->emplace(key + child_key, node_child);
    parent.children->erase(key);
  }
}

Node& AddChild(Node& node, std::string_view key, bool marker, const Data* data,
    ChildrenPtr children = std::make_shared<Children>()) {
  return node.children->emplace(key, Node{marker, data, children}).first->second;
}

void DeleteNode(Node& parent, const std::string& key) {
  auto& node = parent.children->at(key);
  if (node.marker) {
    if (node.children->empty()) {
      parent.children->erase(key);
    } else {
      UnmarkNode(node);
      CompressNode(parent, key);
    }
  }
}

Result FindElement(const Node& parent, std::string_view key) {
  auto matcher = FindSimilarKey(parent.children, key);
  if (matcher) {
    const auto& node = parent.children->at(matcher.key);
    return {node.marker, node.data};
  }
  if (matcher.Partial()) {
    const auto& node = parent.children->at(matcher.key);
    return FindElement(node, matcher.Tail());
  }
  return {false, nullptr};
}

void InsertElement(Node& parent, std::string_view key, const Data* data) {
  auto matcher = FindSimilarKey(parent.children, key);
  if (!matcher) {
    AddChild(parent, key, true, data);
  }
  else if (matcher) {
    auto& node = parent.children->at(matcher.key);
    // If key already marked, data will be replace by new one.
    MarkNode(node, data);
  }
  else if (matcher.Partial()) {
    auto& node = parent.children->at(matcher.key);
    InsertElement(node, matcher.Tail(), data);
  }
  else if (matcher.IsPrefix()) {
    auto& node = AddChild(parent, key, true, data);
    const auto& child = parent.children->at(matcher.key);
    node.children->emplace(matcher.Sufix(), child);
    parent.children->erase(matcher.key);
  }
  else {
    auto& node = AddChild(parent, matcher.Prefix(), false, nullptr);
    AddChild(node, matcher.Tail(), true, data);
    const auto& child = parent.children->at(matcher.key);
    node.children->emplace(matcher.Sufix(), child);
    parent.children->erase(matcher.key);
  }
}

void RemoveElement(Node& parent, std::string_view key) {
  auto matcher = FindSimilarKey(parent.children, key);
  if (matcher) {
    DeleteNode(parent, matcher.key);
  }
  else if (matcher.Partial()) {
    auto& node = parent.children->at(matcher.key);
    RemoveElement(node, matcher.Tail());
    if (!node.marker) {
      CompressNode(parent, matcher.key);
    }
  }
}

}  // namespace

Node& Node::operator[](const std::string& key) {
  return (*children)[key];
}

void Trie::Insert(std::string_view key, const Data* data) {
  InsertElement(*this, key, data);
}

Result Trie::Find(std::string_view key) const {
  return FindElement(*this, key);
}

void Trie::Remove(std::string_view key) {
  RemoveElement(*this, key);
}

std::ostream& operator<<(std::ostream& out, const Trie& tree) {
  return PrintChildren(out, reinterpret_cast<const Node&>(tree), 0);
}
