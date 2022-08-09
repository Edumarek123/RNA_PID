#!/bin/bash

clear


g++ -o model main.cpp ../matriz_lib.cpp ../rede_neural_lib.cpp

# ./model

valgrind --leak-check=full --track-origins=yes -v model

rm model

$SHELL
