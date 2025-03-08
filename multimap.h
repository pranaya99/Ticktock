#ifndef MULTIMAP_H_
#define MULTIMAP_H_

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>

template <typename K, typename V>
class Multimap {
 public:
  Multimap() : root(nullptr), cur_size(0) {}
  
  unsigned int Size() {
    return cur_size;
  }
  
  V Get(const K& key) {
    Node* n = Get(root.get(), key);
    if (!n || n->values.empty()) throw std::runtime_error("Error: cannot find key or no values");
    return n->values[0];  // Return first value only
  }
  
  const V& GetFirst(const K& key) {
    Node* n = Get(root.get(), key);
    if (!n || n->values.empty()) throw std::runtime_error("Error: cannot find key or no values");
    return n->values[0];  // Return first value as reference
  }
  
  std::vector<V> GetAll(const K& key) {
    Node* n = Get(root.get(), key);
    if (!n) throw std::runtime_error("Error: cannot find key");
    return n->values;  // Return entire vector of values
  }
  
  bool Contains(const K& key) {
    return Get(root.get(), key) != nullptr;
  }
  
  const K& Min() {
    if (!root) throw std::runtime_error("Error: tree is empty");
    return Min(root.get())->key;
  }
  
  const K& Max() {
    if (!root) throw std::runtime_error("Error: tree is empty");
    Node* current = root.get();
    while (current->right) {
      current = current->right.get();
    }
    return current->key;
  }
  
  void Insert(const K& key, const V& value) {
    Insert(root, key, value);
    root->color = BLACK;
    cur_size++;
  }
  
  void Remove(const K& key) {
    if (!Contains(key)) return;
    Remove(root, key);
    if (root) root->color = BLACK;
    cur_size--;
  }
  
  void Print() {
    Print(root.get());
    std::cout << std::endl;
  }

 private:
  enum Color { RED, BLACK };
  struct Node {
    K key;
    std::vector<V> values;  // Store a list of values for the same key
    Color color;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    
    Node(const K& k, const std::vector<V>& vals, Color c) 
        : key(k), values(vals), color(c), left(nullptr), right(nullptr) {}
  };
  
  std::unique_ptr<Node> root;
  unsigned int cur_size;

  Node* Get(Node* n, const K& key) {
    while (n) {
      if (key == n->key) return n;
      if (key < n->key) n = n->left.get();
      else n = n->right.get();
    }
    return nullptr;
  }
  
  Node* Min(Node* n) {
    if (!n) return nullptr;
    Node* current = n;
    while (current->left) {
      current = current->left.get();
    }
    return current;
  }
  
  void Insert(std::unique_ptr<Node>& n, const K& key, const V& value) {
    if (!n) {
      n = std::unique_ptr<Node>(new Node(key, std::vector<V>{value}, RED));
      return;
    }
    
    if (key < n->key) {
      Insert(n->left, key, value);
    } else if (key > n->key) {
      Insert(n->right, key, value);
    } else {
      // If key already exists, just add the value to the vector
      n->values.push_back(value);
      return;
    }

    FixUp(n);
  }
  
  void Remove(std::unique_ptr<Node>& n, const K& key) {
    if (!n) return;

    if (key < n->key) {
      if (!IsRed(n->left.get()) && !IsRed(n->left->left.get()))
        MoveRedLeft(n);
      Remove(n->left, key);
    } else {
      if (IsRed(n->left.get())) RotateRight(n);

      if (key == n->key && !n->right) {
        n.reset();
        return;
      }

      if (!IsRed(n->right.get()) && !IsRed(n->right->left.get()))
        MoveRedRight(n);

      if (key == n->key) {
        // Just remove the first value from the vector
        if (n->values.size() > 1) {
          n->values.erase(n->values.begin());
          return;
        }
        
        // If only one value left, remove the node
        Node* minNode = Min(n->right.get());
        n->key = minNode->key;
        n->values = minNode->values;
        DeleteMin(n->right);
      } else {
        Remove(n->right, key);
      }
    }

    FixUp(n);
  }
  
  void Print(Node* n) {
    if (!n) return;
    Print(n->left.get());
    std::cout << "<" << n->key << ": ";
    for (const auto& val : n->values) {
      std::cout << val << " ";
    }
    std::cout << "> ";
    Print(n->right.get());
  }

  // Helper functions for Red-Black tree balancing
  
  bool IsRed(Node* n) {
    if (!n) return false;
    return (n->color == RED);
  }
  
  void FlipColors(Node* n) {
    n->color = static_cast<Color>(!n->color);
    n->left->color = static_cast<Color>(!n->left->color);
    n->right->color = static_cast<Color>(!n->right->color);
  }
  
  void RotateRight(std::unique_ptr<Node>& prt) {
    std::unique_ptr<Node> chd(std::move(prt->left));
    prt->left = std::move(chd->right);
    chd->color = prt->color;
    prt->color = RED;
    chd->right = std::move(prt);
    prt = std::move(chd);
  }
  
  void RotateLeft(std::unique_ptr<Node>& prt) {
    std::unique_ptr<Node> chd(std::move(prt->right));
    prt->right = std::move(chd->left);
    chd->color = prt->color;
    prt->color = RED;
    chd->left = std::move(prt);
    prt = std::move(chd);
  }
  
  void FixUp(std::unique_ptr<Node>& n) {
    if (IsRed(n->right.get()) && !IsRed(n->left.get())) RotateLeft(n);
    if (IsRed(n->left.get()) && IsRed(n->left->left.get())) RotateRight(n);
    if (IsRed(n->left.get()) && IsRed(n->right.get())) FlipColors(n.get());
  }
  
  void MoveRedRight(std::unique_ptr<Node>& n) {
    FlipColors(n.get());
    if (n->left && IsRed(n->left->left.get())) {
      RotateRight(n);
      FlipColors(n.get());
    }
  }
  
  void MoveRedLeft(std::unique_ptr<Node>& n) {
    FlipColors(n.get());
    if (n->right && n->right->left && IsRed(n->right->left.get())) {
      RotateRight(n->right);
      RotateLeft(n);
      FlipColors(n.get());
    }
  }
  
  void DeleteMin(std::unique_ptr<Node>& n) {
    if (!n->left) {
      n.reset();
      return;
    }

    if (!IsRed(n->left.get()) && !IsRed(n->left->left.get()))
      MoveRedLeft(n);

    DeleteMin(n->left);
    FixUp(n);
  }
};

#endif  // MULTIMAP_H_
