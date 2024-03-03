#pragma once

#include <compare>
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <iostream>

template <std::three_way_comparable K, typename V>
class RBTree {
    struct Node {
        K key;
        V value;
        Node *link[2];
        uint8_t red;

        ~Node() {
            delete link[0];
            delete link[1];
        }
    };
    
    Node *root_;

    static bool is_red(Node *node) {
        if (node != nullptr) {
            return node->red == 1;
        }
        return false;
    }

    static Node *single_rot(Node *root, uint8_t dir) {
        assert(root != nullptr);
        assert(dir == 0 || dir == 1);
        Node* storage = root->link[!dir];

        root->link[!dir] = storage->link[dir];
        storage->link[dir] = root;

        root->red = 1;
        storage->red = 0;

        return storage;
    }

    static Node *double_rot(Node *root, uint8_t dir) {
        assert(root != nullptr);
        assert(dir == 0 || dir == 1);
        root->link[!dir] = single_rot(root->link[!dir], !dir);
        return single_rot(root, dir);
    }

    // returns 0 if tree is incorrect BST
    static size_t black_height(Node *root) {
        size_t l_height, r_height;

        if (root == nullptr) {
            return 1;
        }
        else {
            Node* ln = root->link[0];
            Node* rn = root->link[1];

#ifndef NDEBUG
            if (is_red(root)) {
                if (is_red(ln) || is_red(rn)) {
                    std::cout << "ERROR: 2 red parents\n";
                    return 0;
                }
            }
#endif

            l_height = black_height(ln);
            r_height = black_height(rn);
#ifndef NDEBUG
            if ((ln != nullptr && ln->key >= root->key) ||
                (rn != nullptr && rn->key <= root->key)) {
                std::cout << "ERROR: Not a BST\n";
                return 0;
            }

            if (l_height != 0 && r_height != 0 && l_height != r_height) {
                std::cout << "ERROR: Black height mismatch\n";
                return 0;
            }

            if (l_height != 0 && r_height != 0) {
#endif
                return is_red(root) ? l_height : l_height + 1;
#ifndef NDEBUG
            }
            else {
                return 0;
            }
#endif
        }
    }
    
public:
    RBTree()
    : root_{ nullptr } {};

    ~RBTree() {
        delete root_;
    }

    void insert(const K &key, const V &value) {
        if (root_ == nullptr) {
            root_ = new Node{ key, value, { nullptr, nullptr }, 1 };
        }
        else {
            //TODO may be optimise K{} and V{} ??
            Node head = { K{}, V{}, { nullptr, nullptr }, 1 };
            Node* g, * t, * p, * q;
            uint8_t dir, last_dir;

            dir = 0;
            t = &head;
            g = p = NULL;
            q = t->link[1] = root_;

            while (true) {
                if (q == nullptr) {
                    p->link[dir] = q = new Node{ key, value, { nullptr, nullptr }, 1 };
                }
                else if (is_red(q->link[0]) && is_red(q->link[1])) {
                    q->red = 1;
                    q->link[0]->red = 0;
                    q->link[1]->red = 0;
                }

                if (is_red(q) && is_red(p)) {
                    uint8_t dir1 = t->link[1] == g;

                    if (q == p->link[last_dir]) {
                        t->link[dir1] = single_rot(g, !last_dir);
                    }
                    else {
                        t->link[dir1] = double_rot(g, !last_dir);
                    }
                }

                if (q->key == key) {
                    break;
                }

                last_dir = dir;
                dir = q->key < key;

                if (g != nullptr) {
                    t = g;
                }

                g = p;
                p = q;
                q = q->link[dir];
            }

            root_ = head.link[1];
            head.link[0] = nullptr;
            head.link[1] = nullptr;
        }

        root_->red = 0;

        assert(black_height(root_) != 0);
    }

    void remove(const K &key) {
        if (root_ != nullptr) {
            Node head = { K{}, V{}, { nullptr, nullptr }, 0 };
            Node* p, * q, * g, * f;
            uint8_t dir = 1;

            f = nullptr;
            q = &head;
            g = p = nullptr;
            q->link[1] = root_;

            while (q->link[dir] != nullptr) {
                uint8_t last_dir = dir;

                g = p;
                p = q;
                q = q->link[dir];
                dir = q->key < key;

                if (q->key == key) {
                    f = q;
                }

                if (!is_red(q) && !is_red(q->link[dir])) {
                    if (is_red(q->link[!dir])) {
                        p = p->link[last_dir] = single_rot(q, dir);
                    }
                    else if (!is_red(q->link[!dir])) {
                        Node* s = p->link[!last_dir];

                        if (s != nullptr) {
                            if (!is_red(s->link[!last_dir]) && !is_red(s->link[last_dir])) {
                                p->red = 0;
                                s->red = 1;
                                q->red = 1;
                            }
                            else {
                                uint8_t dir1 = g->link[1] == p;

                                if (is_red(s->link[last_dir])) {
                                    g->link[dir1] = double_rot(p, last_dir);
                                }
                                else if (is_red(s->link[!last_dir])) {
                                    g->link[dir1] = single_rot(p, last_dir);
                                }

                                q->red = g->link[dir1]->red = 1;
                                g->link[dir1]->link[0]->red = 0;
                                g->link[dir1]->link[1]->red = 0;
                            }
                        }
                    }
                }
            }

            if (f != nullptr) {
                f->key = q->key;
                f->value = q->value;
                p->link[p->link[1] == q] = q->link[q->link[0] == nullptr];
                delete q;
            }

            root_ = head.link[1];
            head.link[0] = nullptr;
            head.link[1] = nullptr;
            if (root_ != nullptr) {
                root_->red = 0;
            }
        }

        assert(black_height(root_) != 0);
    }

    bool contains(const K &key) const {
        Node* node = root_;
        while (node != nullptr) {
            if (key < node->key) {
                node = node->link[0];
            }
            else if (key > node->key) {
                node = node->link[1];
            }
            else {
                return true;
            }
        }
        return false;
    }
};
