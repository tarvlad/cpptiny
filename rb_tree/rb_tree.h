#pragma once

#include <cassert>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <concepts>

template <std::three_way_comparable K, typename V>
class RBTree {
    enum Color {
        RED, BLACK
    };

    struct Node {
        K key;
        V value;
        Node *link[2];
        Color color;

        Node(const K &k, const V &v, Color c) 
            : key{ k }, value{ v }, link{ nullptr, nullptr }, color{ c } {
        }

        Node(const K &k, const V &v, Node *l, Node *r, Color c)
            : key{ k }, value{ v }, link{ l, r }, color{ c } {}

        Node(const Node &n)
            : key{ n.key }, value{ n.value }, link{ nullptr, nullptr }, color{ n.color } {
            
            if (n.link[0] != nullptr) {
                link[0] = new Node{ n.link[0] };
            }
            if (n.link[1] != nullptr) {
                link[1] = new Node{ n.link[1] };
            }
        }

        Node &operator=(const Node &n) {
            key = n.key;
            value = n.value;
            
            delete link[0];
            delete link[1];
            
            link[0] = n.link[0];
            link[1] = n.link[1];
            return *this;
        }

        Node &operator=(Node &&n) {
            key = std::move(n.key);
            value = std::move(n.value);
            
            delete link[0];
            delete link[1];

            link[0] = n.link[0];
            link[1] = n.link[1];
            
            n.link[0] = nullptr;
            n.link[1] = nullptr;
            return *this;
        }

        ~Node() {
            delete link[0];
            delete link[1];
        }
    };

    Node *root_;

    static bool is_red(Node *node) {
        if (node != nullptr) {
            return node->color == RED;
        }
        return false;
    }

    static Node *single_rot(Node *root, uint8_t dir) {
        assert(root != nullptr);
        assert(dir == 0 || dir == 1);
        Node *storage = root->link[!dir];

        root->link[!dir] = storage->link[dir];
        storage->link[dir] = root;

        root->color = RED;
        storage->color = BLACK;

        return storage;
    }

    static Node *double_rot(Node *root, uint8_t dir) {
        assert(root != nullptr);
        assert(dir == 0 || dir == 1);
        root->link[!dir] = single_rot(root->link[!dir], !dir);
        return single_rot(root, dir);
    }

    // returns 0 if tree is incorrect BST and assertions enabled
    static size_t black_height(Node *root) {
        size_t l_height, r_height;

        if (root == nullptr) {
            return 1;
        } else {
            Node *ln = root->link[0];
            Node *rn = root->link[1];

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
            } else {
                return 0;
            }
#endif
        }
    }

public:
    RBTree()
        : root_{ nullptr } {};

    RBTree(const RBTree &t) 
        : root_{ new Node{ *t.root_ } } {
    }

    ~RBTree() {
        delete root_;
    }

    RBTree &operator=(const RBTree &t) {
        delete root_;
        root_ = new Node{ *t.root };
        return *this;
    }

    RBTree &operator=(RBTree &&t) {
        delete root_;
        root_ = t.root_;
        t.root_ = nullptr;
        return *this;
    }

    void insert(const K &key, const V &value) {
        if (root_ == nullptr) {
            root_ = new Node{ key, value, BLACK };
        } else {
            //TODO may be optimise K{} and V{} ??
            Node head = { K{}, V{}, RED };
            Node *g, *t, *p, *q;
            uint8_t dir, last_dir;

            dir = 0;
            t = &head;
            g = p = NULL;
            q = t->link[1] = root_;

            while (true) {
                if (q == nullptr) {
                    p->link[dir] = q = new Node{ key, value, RED };
                } else if (is_red(q->link[0]) && is_red(q->link[1])) {
                    q->color = RED;
                    q->link[0]->color = BLACK;
                    q->link[1]->color = BLACK;
                }

                if (is_red(q) && is_red(p)) {
                    uint8_t dir1 = t->link[1] == g;

                    if (q == p->link[last_dir]) {
                        t->link[dir1] = single_rot(g, !last_dir);
                    } else {
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

        root_->color = BLACK;

        assert(black_height(root_) != 0);
    }

    void remove(const K &key) {
        if (root_ != nullptr) {
            Node head = { K{}, V{}, BLACK };
            Node *p, *q, *g, *f;
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
                    } else if (!is_red(q->link[!dir])) {
                        Node *s = p->link[!last_dir];

                        if (s != nullptr) {
                            if (!is_red(s->link[!last_dir]) && !is_red(s->link[last_dir])) {
                                p->color = BLACK;
                                s->color = RED;
                                q->color = RED;
                            } else {
                                uint8_t dir1 = g->link[1] == p;

                                if (is_red(s->link[last_dir])) {
                                    g->link[dir1] = double_rot(p, last_dir);
                                } else if (is_red(s->link[!last_dir])) {
                                    g->link[dir1] = single_rot(p, last_dir);
                                }

                                q->color = g->link[dir1]->color = RED;
                                g->link[dir1]->link[0]->color = BLACK;
                                g->link[dir1]->link[1]->color = BLACK;
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
                root_->color = BLACK;
            }
        }

        assert(black_height(root_) != 0);
    }

    bool contains(const K &key) const {
        Node *node = root_;
        while (node != nullptr) {
            if (key < node->key) {
                node = node->link[0];
            } else if (key > node->key) {
                node = node->link[1];
            } else {
                return true;
            }
        }
        return false;
    }
};
