#ifndef MULTIMAP_H_
#define MULTIMAP_H_

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

template <typename K, typename V>
class Multimap {
 public:
  // Return size of tree
  unsigned int Size();
  // Return first value associated with @key
  const V& Get(const K& key);
  // Return whether @key is found in tree
  bool Contains(const K& key);
  // Return max key in tree
  const K& Max();
  // Return min key in tree
  const K& Min();
  // Insert @key-value pair in tree
  void Insert(const K &key, const V &value);
  // Remove @key-value pair from tree
  void Remove(const K &key);
  // Print tree in-order
  void Print();

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

  // Helper methods
  Node* Get(Node *n, const K &key);
  Node* Min(Node *n);
  void Insert(std::unique_ptr<Node> &n, const K &key, const V &value);
  void Remove(std::unique_ptr<Node> &n, const K &key);
  void Print(Node *n);

  // Balancing methods
  bool IsRed(Node *n);
  void FlipColors(Node *n);
  void RotateRight(std::unique_ptr<Node> &prt);
  void RotateLeft(std::unique_ptr<Node> &prt);
  void FixUp(std::unique_ptr<Node> &n);
};



#endif
