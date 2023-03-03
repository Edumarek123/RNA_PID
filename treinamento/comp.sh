#!/bin/bash

clear


g++ -o model main.cpp src/matriz_lib/matriz_lib.cpp src/rede_neural_lib/rede_neural_lib.cpp

./model

# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./model

rm model
