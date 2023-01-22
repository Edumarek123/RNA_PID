#!/bin/bash

clear


g++ -o model XOR.cpp /home/dudu/Arduino/libraries/RNA_PID/matriz_lib.cpp /home/dudu/Arduino/libraries/RNA_PID/rede_neural_lib.cpp /home/dudu/Arduino/libraries/RNA_PID/PID_lib.cpp

./model

# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./model

rm model
