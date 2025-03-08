#ifndef MULTIMAP_H_
#define MULTIMAP_H_

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Multimap class using Red-Black Tree for ordered key-value pairs
template <typename K, typename V>
class Multimap {
 public:
  unsigned int Size() const;
  V Get(const K& key) const;
  std::vector<V> GetAll(const K& key) const;
  const V& GetFirst(const K& key) const;
  bool Contains(const K& key) const;
  const K& Max() const;
  const K& Min() const;
  void Insert(const K& key, const V& value);
  void Remove(const K& key);
  void Print() const;

 private:
  enum Color { RED, BLACK };
  struct Node {
    K key;
    std::vector<V> values;
    bool color;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
  };

  std::unique_ptr<Node> root;
  unsigned int cur_size = 0;

  Node* Get(Node* n, const K& key) const;
  Node* Min(Node* n) const;
  void Insert(std::unique_ptr<Node>* n, const K& key, const V& value);
  void Remove(std::unique_ptr<Node>* n, const K& key);
  void Print(Node* n) const;

  bool IsRed(const Node* n) const;
  void FlipColors(Node* n);
  void RotateRight(std::unique_ptr<Node>* prt);
  void RotateLeft(std::unique_ptr<Node>* prt);
  void FixUp(std::unique_ptr<Node>* n);
  void MoveRedRight(std::unique_ptr<Node>* n);
  void MoveRedLeft(std::unique_ptr<Node>* n);
  void DeleteMin(std::unique_ptr<Node>* n);
};

// Returns the size of the multimap.
template <typename K, typename V>
unsigned int Multimap<K, V>::Size() const {
  return cur_size;
}

// Finds the node for the given key.
template <typename K, typename V>
typename Multimap<K, V>::Node* Multimap<K, V>::Get(Node* n,
                                                    const K& key) const {
  while (n) {
    if (key == n->key) return n;
    if (key < n->key) {
      n = n->left.get();
    } else {
      n = n->right.get();
    }
  }
  return nullptr;
}

// Retrieves the first value for a given key.
template <typename K, typename V>
V Multimap<K, V>::Get(const K& key) const {
  Node* n = Get(root.get(), key);
  if (!n || n->values.empty()) {
    throw std::runtime_error("Error: cannot find key");
  }
  return n->values[0];
}

// Gets the first value for a key
template <typename K, typename V>
const V& Multimap<K, V>::GetFirst(const K& key) const {
  Node* n = Get(root.get(), key);
  if (!n || n->values.empty()) {
    throw std::runtime_error("Error: cannot find key");
  }
  return n->values[0];
}

// Retrieves all values associated with a key.
template <typename K, typename V>
std::vector<V> Multimap<K, V>::GetAll(const K& key) const {
  Node* n = Get(root.get(), key);
  if (!n) {
    throw std::runtime_error("Error: cannot find key");
  }
  return n->values;
}

// Checks if the key exists in the multimap.
template <typename K, typename V>
bool Multimap<K, V>::Contains(const K& key) const {
  return Get(root.get(), key) != nullptr;
}

// Returns the maximum key in the multimap.
template <typename K, typename V>
const K& Multimap<K, V>::Max() const {
  Node* n = root.get();
  while (n->right) {
    n = n->right.get();
  }
  return n->key;
}

// Returns the minimum key in the multimap.
template <typename K, typename V>
const K& Multimap<K, V>::Min() const {
  return Min(root.get())->key;
}

// Finds the minimum node in a subtree.
template <typename K, typename V>
typename Multimap<K, V>::Node* Multimap<K, V>::Min(Node* n) const {
  return n->left ? Min(n->left.get()) : n;
}

// Checks if a node is red.
template <typename K, typename V>
bool Multimap<K, V>::IsRed(const Node* n) const {
  return n && (n->color == RED);
}

// Flips colors to maintain Red-Black properties.
template <typename K, typename V>
void Multimap<K, V>::FlipColors(Node* n) {
  n->color = !n->color;
  n->left->color = !n->left->color;
  n->right->color = !n->right->color;
}

// Rotates the subtree right.
template <typename K, typename V>
void Multimap<K, V>::RotateRight(std::unique_ptr<Node>* prt) {
  std::unique_ptr<Node> chd(std::move((*prt)->left));
  (*prt)->left = std::move(chd->right);
  chd->color = (*prt)->color;
  (*prt)->color = RED;
  chd->right = std::move(*prt);
  *prt = std::move(chd);
}

// Rotates the subtree left.
template <typename K, typename V>
void Multimap<K, V>::RotateLeft(std::unique_ptr<Node>* prt) {
  std::unique_ptr<Node> chd(std::move((*prt)->right));
  (*prt)->right = std::move(chd->left);
  chd->color = (*prt)->color;
  (*prt)->color = RED;
  chd->left = std::move(*prt);
  *prt = std::move(chd);
}

// Inserts a key-value pair into the multimap.
template <typename K, typename V>
void Multimap<K, V>::Insert(const K& key, const V& value) {
  Insert(&root, key, value);
  cur_size++;
  root->color = BLACK;
}

// Private insert helper function.
template <typename K, typename V>
void Multimap<K, V>::Insert(std::unique_ptr<Node>* n,
                            const K& key, const V& value) {
  if (!*n) {
    // C++11 compatible way to create a unique_ptr
    std::unique_ptr<Node> new_node(new Node());
    new_node->key = key;
    new_node->values.push_back(value);
    new_node->color = RED;
    *n = std::move(new_node);
  } else if (key < (*n)->key) {
    Insert(&((*n)->left), key, value);
  } else if (key > (*n)->key) {
    Insert(&((*n)->right), key, value);
  } else {
    (*n)->values.push_back(value);
    cur_size--;
  }
  FixUp(n);
}

// Fix up the tree balance after insertion
template <typename K, typename V>
void Multimap<K, V>::FixUp(std::unique_ptr<Node>* n) {
  // If right child is red and left child is black, rotate left
  if (IsRed((*n)->right.get()) && !IsRed((*n)->left.get())) {
    RotateLeft(n);
  }
  // If left child and left-left grandchild are red, rotate right
  if (IsRed((*n)->left.get()) && IsRed((*n)->left->left.get())) {
    RotateRight(n);
  }
  // If both children are red, flip colors
  if (IsRed((*n)->left.get()) && IsRed((*n)->right.get())) {
    FlipColors(n->get());
  }
}

// Prints the multimap in-order.
template <typename K, typename V>
void Multimap<K, V>::Print() const {
  Print(root.get());
  std::cout << std::endl;
}

// Private print helper function.
template <typename K, typename V>
void Multimap<K, V>::Print(Node* n) const {
  if (!n) return;
  Print(n->left.get());
  std::cout << "<" << n->key << ": ";
  for (const auto& val : n->values) {
    std::cout << val << " ";
  }
  std::cout << "> ";
  Print(n->right.get());
}

// Move red nodes to the right
template <typename K, typename V>
void Multimap<K, V>::MoveRedRight(std::unique_ptr<Node>* n) {
  FlipColors(n->get());
  if (IsRed((*n)->left->left.get())) {
    RotateRight(n);
    FlipColors(n->get());
  }
}

// Move red nodes to the left
template <typename K, typename V>
void Multimap<K, V>::MoveRedLeft(std::unique_ptr<Node>* n) {
  FlipColors(n->get());
  if (IsRed((*n)->right->left.get())) {
    RotateRight(&((*n)->right));
    RotateLeft(n);
    FlipColors(n->get());
  }
}

// Delete the minimum key
template <typename K, typename V>
void Multimap<K, V>::DeleteMin(std::unique_ptr<Node>* n) {
  if (!(*n)->left) {
    *n = nullptr;
    return;
  }
  if (!IsRed((*n)->left.get()) && !IsRed((*n)->left->left.get())) {
    MoveRedLeft(n);
  }
  DeleteMin(&((*n)->left));
  FixUp(n);
}
// Remove a key and all its values
template <typename K, typename V>
void Multimap<K, V>::Remove(const K& key) {
  if (!Contains(key)) return;
  Remove(&root, key);
  if (root) root->color = BLACK;
}

// Private remove helper function
template <typename K, typename V>
void Multimap<K, V>::Remove(std::unique_ptr<Node>* n, const K& key) {
  if (key < (*n)->key) {
    // Move red left if needed
    if (!IsRed((*n)->left.get()) && !IsRed((*n)->left->left.get())) {
      MoveRedLeft(n);
    }
    Remove(&((*n)->left), key);
  } else {
    // If left child is red, rotate right
    if (IsRed((*n)->left.get())) {
      RotateRight(n);
    }
    // Found the key and no right child
    if (key == (*n)->key && !(*n)->right) {
      *n = nullptr;
      return;
    }
    // Move red right if needed
    if (!IsRed((*n)->right.get()) && !IsRed((*n)->right->left.get())) {
      MoveRedRight(n);
    }
    // Found the key
    if (key == (*n)->key) {
      // Find successor (min of right subtree)
      Node* successor = Min((*n)->right.get());
      (*n)->key = successor->key;
      (*n)->values = std::move(successor->values);
      // Delete the successor
      DeleteMin(&((*n)->right));
    } else {
      // Continue searching in right subtree
      Remove(&((*n)->right), key);
    }
  }
  // Fix up the tree
  FixUp(n);
}
#endif  // MULTIMAP_H_
