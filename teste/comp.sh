#!/bin/bash

clear


g++ -o model main.cpp ../libs/matriz_lib.cpp ../libs/rede_neural_lib.cpp #../libs/PID_lib.cpp

./model

# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./model

rm model
