#include "trie.h"

#include <algorithm>
#include <iterator>
#include <string_view>
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

std::pair<const std::string&, size_t>
FindSimilarKey(const Children& children, std::string_view key) {
  for (const auto& c: *children) {
    auto pos = Compare(c.first, key);
    if (pos > 0) {
      return {c.first, pos};
    }
  }
  return {{}, 0u};
}

std::ostream& PrintChildren(std::ostream& out , const Children& children, int deep) {
  std::string prefix(deep, '-');
  for (const auto& c: *children) {
    out << prefix << c.first << "\n";
    PrintChildren(out, c.second.children, deep + 1);
  }
  return out;
}
}  // namespace
//
//Node::Node(bool marker, const Data* data, const Children& children)
//    : marker{marker},
//      data{data},
//      children{children} {}

Node& Node::operator[](const std::string& key) {
  return (*children)[key];
}

Node& Node::AddChild(const std::string& key, bool marker, const Data* data,
    const Children& children) {
  this->children->emplace(key, Node{marker, data, children});
  return (*this->children)[key];
}

void Node::MoveChild(const std::string& key, Node& node) {
  children->emplace(key, node);
}

void Node::MarkChild(const std::string& key, const Data* data) {
  auto& node = (*children)[key];
  node.marker = true;
  node.data = data;
}

//void Node::UnmarkChild(const std::string& key) {
//  auto node = children[key];
//  node.marker = false;
//  node.data = nullptr;
//}

void Node::DeleteChild(const std::string& key) {
  children->erase(key);
}

//void Node::DeleteElement(Children::const_iterator it) {
//  if (it->second.marker) {
//    auto key = it->first;
//    auto node = it->second;
//    if (node.children.empty()) {
//      DeleteChild(key);
//    } else if (node.children.size() == 1) {
//      auto child_key = node.children.begin()->first;
//      auto child_node = node.children.begin()->second;
//      MoveChild(key + child_key, child_node);
//      node.DeleteChild(child_key);
//      DeleteChild(key);
//    } else {
//      UnmarkChild(key);
//    }
//  }
//}

void Node::Insert(std::string key, const Data* data) {
  const auto& [k, pos] = FindSimilarKey(children, key);
  if (pos == 0) {
    AddChild(key, true, data);
  } else if (k == key) {
    MarkChild(k, data);
  } else if (k == key.substr(0, pos)) {
    (*children)[k].Insert(key.substr(pos), data);
  } else if (k.substr(0, pos) == key) {
    auto& node = AddChild(key, true, data);
    node.MoveChild(k.substr(pos), (*children)[k]);
    DeleteChild(k);
  } else {
    auto& node = AddChild(k.substr(0, pos), false, nullptr);
    node.AddChild(key.substr(pos), true, data);
    node.MoveChild(k.substr(pos), (*children)[k]);
    DeleteChild(k);
  }
}

Node::Result Node::Find(std::string_view key) {
  const auto& [k, pos] = FindSimilarKey(children, key);
  if (pos > 0) {
    auto& node = (*children)[k];
    if (k == key) {  // the same
      return {node.marker, node.data};
    } else if (k == key.substr(0, pos)) {  // it->first is begin of key
      return node.Find(key.substr(pos));
    }
  }
  return {false, nullptr};
}

//void Node::Compress(const std::string& key) {
//  auto node = children[key];
//  if (!node.marker && node.children.size() == 1) {
//    auto key_child = node.children.begin()->first;
//    auto node_child = node.children.begin()->second;
//    MoveChild(key + key_child, node_child);
//    DeleteChild(key);
//  }
//}
//
//void Node::RemoveElement(std::string_view key) {
//  const auto& [it, pos] = FindSimilarKey(children, key);
//  if (pos > 0) {
//    if (it->first == key) {
//      DeleteElement(it);
//    } else if (it->first == key.substr(0, pos)) {
//      it->second.Remove(key.substr(pos));
//      Compress(it->first);
//    }
//  }
//}
//
//void Node::Remove(std::string_view key) {
//  RemoveElement(key);
//}
//
std::ostream& operator<<(std::ostream& out, const Trie& tree) {
  return PrintChildren(out, tree.children, 0);
}
