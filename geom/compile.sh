#!/bin/bash


clang++ -Wall -ggdb -std=c++26 -march=native main.cpp -o geom_tests -fsanitize=address
