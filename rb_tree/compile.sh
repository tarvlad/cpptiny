#!/bin/bash


clang++ -Wall -ggdb -std=c++26 -march=native main.cpp -o rb_tree_tests -fsanitize=address
