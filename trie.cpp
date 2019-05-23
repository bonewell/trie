#include "trie.h"

#include <algorithm>
#include <utility>

namespace {
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

std::pair<std::string, size_t>
FindSimilarKey(ChildrenPtr children, std::string_view key) {
  for (const auto& c: *children) {
    auto pos = Compare(c.first, key);
    if (pos > 0) {
      return {c.first, pos};
      // It is not posible to have few similar keys on the same level
    }
  }
  return {{}, 0u};
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
 * Compress a child of the node with child of the child if:
 * the child has one child only
 * the child is not marked (not presented in tree)
 */
void Compress(Node& parent, const std::string& key) {
  auto& node = (*parent.children)[key];
  if (!node.marker && node.children->size() == 1) {
    const auto& key_child = node.children->begin()->first;
    const auto& node_child = node.children->begin()->second;
    parent.children->emplace(key + key_child, node_child);
    parent.children->erase(key);
  }
}

Node& AddChild(Node& node, const std::string& key, bool marker, const Data* data,
    ChildrenPtr children = std::make_shared<Children>()) {
  node.children->emplace(key, Node{marker, data, children});
  return (*node.children)[key];
}

void DeleteNode(Node& parent, const std::string& key) {
  auto& node = (*parent.children)[key];
  if (node.marker) {
    if (node.children->empty()) {
      parent.children->erase(key);
    } else if (node.children->size() == 1) {
      // Compress
      const auto& child_key = node.children->begin()->first;
      const auto& child_node = node.children->begin()->second;
      parent.children->emplace(key + child_key, child_node);
      parent.children->erase(key);
    } else {
      UnmarkNode(node);
    }
  }
}

Result FindElement(const Node& parent, std::string_view key) {
  const auto& [k, pos] = FindSimilarKey(parent.children, key);
  if (pos > 0) {
    const auto& node = (*parent.children)[k];
    if (k == key) {  // the same
      return {node.marker, node.data};
    } else if (k == key.substr(0, pos)) {  // it->first is begin of key
      return FindElement(node, key.substr(pos));
    }
  }
  return {false, nullptr};
}

void InsertElement(Node& parent, const std::string& key, const Data* data) {
  const auto& [k, pos] = FindSimilarKey(parent.children, key);
  if (pos == 0) {
    AddChild(parent, key, true, data);
  } else if (k == key) {
    MarkNode((*parent.children)[k], data);
  } else if (k == key.substr(0, pos)) {
    InsertElement((*parent.children)[k], key.substr(pos), data);
  } else if (k.substr(0, pos) == key) {
    auto& node = AddChild(parent, key, true, data);
    node.children->emplace(k.substr(pos), (*parent.children)[k]);
    parent.children->erase(k);
  } else {
    auto& node = AddChild(parent, k.substr(0, pos), false, nullptr);
    AddChild(node, key.substr(pos), true, data);
    node.children->emplace(k.substr(pos), (*parent.children)[k]);
    parent.children->erase(k);
  }
}

void RemoveElement(Node& parent, std::string_view key) {
  const auto& [k, pos] = FindSimilarKey(parent.children, key);
  if (pos > 0) {
    if (k == key) {
      DeleteNode(parent, k);
    } else if (k == key.substr(0, pos)) {
      RemoveElement((*parent.children)[k], key.substr(pos));
      Compress(parent, k);
    }
  }
}

}  // namespace

Node& Node::operator[](const std::string& key) {
  return (*children)[key];
}

void Trie::Insert(std::string key, const Data* data) {
  InsertElement(*this, key, data);
}

Result Trie::Find(std::string_view key) {
  return FindElement(*this, key);
}

void Trie::Remove(std::string_view key) {
  RemoveElement(*this, key);
}

std::ostream& operator<<(std::ostream& out, const Trie& tree) {
  return PrintChildren(out, reinterpret_cast<const Node&>(tree), 0);
}
