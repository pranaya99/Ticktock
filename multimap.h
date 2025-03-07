#ifndef MULTIMAP_H
#define MULTIMAP_H

#include <iostream>
#include <memory>
#include <map>
#include <vector>

enum Color { RED, BLACK };

template <typename K, typename V>
class Multimap {
private:
    struct Node {
        K key;
        std::multimap<K, V> values;
        Color color;
        std::unique_ptr<Node> left, right;
        
        Node(K k, V v) : key(k), color(RED) {
            values.insert({k, v});
        }
    };

    std::unique_ptr<Node> root;

    void RotateLeft(std::unique_ptr<Node>& n) {
        auto temp = std::move(n->right);
        n->right = std::move(temp->left);
        temp->left = std::move(n);
        n = std::move(temp);
        n->color = n->left->color;
        n->left->color = RED;
    }

    void RotateRight(std::unique_ptr<Node>& n) {
        auto temp = std::move(n->left);
        n->left = std::move(temp->right);
        temp->right = std::move(n);
        n = std::move(temp);
        n->color = n->right->color;
        n->right->color = RED;
    }

    void FlipColors(std::unique_ptr<Node>& n) {
        n->color = RED;
        n->left->color = BLACK;
        n->right->color = BLACK;
    }

    void FixUp(std::unique_ptr<Node>& n) {
        if (n->right && n->right->color == RED) {
            RotateLeft(n);
        }
        if (n->left && n->left->color == RED && n->left->left && n->left->left->color == RED) {
            RotateRight(n);
        }
        if (n->left && n->left->color == RED && n->right && n->right->color == RED) {
            FlipColors(n);
        }
    }

    void Insert(std::unique_ptr<Node>& n, const K& key, const V& value) {
        if (!n) {
            n = std::make_unique<Node>(key, value);
            return;
        }

        if (key < n->key) {
            Insert(n->left, key, value);
        } else if (key > n->key) {
            Insert(n->right, key, value);
        } else {
            n->values.insert({key, value});
        }

        FixUp(n);
    }

    Node* FindNode(const K& key) const {
        Node* current = root.get();
        while (current) {
            if (key < current->key) {
                current = current->left.get();
            } else if (key > current->key) {
                current = current->right.get();
            } else {
                return current;
            }
        }
        return nullptr;
    }

public:
    void Insert(const K& key, const V& value) {
        Insert(root, key, value);
        if (root) root->color = BLACK; // Root is always BLACK
    }

    bool Contains(const K& key) const {
        return FindNode(key) != nullptr;
    }

    std::vector<V> Get(const K& key) const {
        std::vector<V> result;
        Node* node = FindNode(key);
        if (node) {
            for (const auto& pair : node->values) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    void PrintInOrder(const std::unique_ptr<Node>& n) const {
        if (!n) return;
        PrintInOrder(n->left);
        std::cout << n->key << ": ";
        for (const auto& val : n->values)
            std::cout << val.second << " ";
        std::cout << "\n";
        PrintInOrder(n->right);
    }

    void Print() const {
        PrintInOrder(root);
    }
};

#endif // MULTIMAP_H
