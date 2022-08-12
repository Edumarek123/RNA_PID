#!/bin/bash

clear


g++ -o model main2.cpp ../libs/matriz_lib.cpp ../libs/rede_neural_lib.cpp ../libs/PID_lib.cpp

./model

# valgrind --leak-check=full --track-origins=yes -v ./model

rm model
