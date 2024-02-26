#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "rb_tree.h"

void test_insert_and_remove_operations() {
    RBTree<int, int> tree;
    std::vector<int> keys_to_insert = {50, 20, 60, 10, 30, 70, 5, 15, 25, 35, 65, 75};
    std::vector<int> keys_to_remove = {30, 10, 60, 50};

    for (int key : keys_to_insert) {
        tree.insert(key, key);
    }

    for (int key : keys_to_insert) {
        if (!tree.contains(key)) {
            std::cerr << "Insertion test failed: key " << key << " not found after insertion.\n";
            return;
        }
    }

    for (int key : keys_to_remove) {
        tree.remove(key);
    }

    for (int key : keys_to_remove) {
        if (tree.contains(key)) {
            std::cerr << "Removal test failed: key " << key << " found after removal.\n";
            return;
        }
    }

    for (int key : keys_to_insert) {
        if (std::find(keys_to_remove.begin(), keys_to_remove.end(), key) == keys_to_remove.end() && !tree.contains(key)) {
            std::cerr << "Removal test failed: non-removed key " << key << " not found after other removals.\n";
            return;
        }
    }

    std::cout << "Test insert-remove operations passed\n";
}

void random_tests() {
    RBTree<int, int> tree;
    const size_t operation_count = 10000;
    std::vector<int> inserted_keys;

    for (size_t i = 0; i < operation_count; ++i) {
        int operation = rand() % 2;
        int key = rand() % operation_count;

        if (operation == 0) {
            tree.insert(key, key);
            inserted_keys.push_back(key);
        } else {
            tree.remove(key);
            inserted_keys.erase(std::remove(inserted_keys.begin(), inserted_keys.end(), key), inserted_keys.end());
        }
    }

    for (int key : inserted_keys) {
        if (!tree.contains(key)) {
            std::cerr << "Random operations test failed: inserted key " << key << " not found.\n";
            return;
        }
    }

    std::cout << "Test random operations passed\n";
}

int main() {
    test_insert_and_remove_operations();
    random_tests();

    return 0;
}
