#include <iostream>
#include "../libs/matriz_lib.hpp"


int main() {
	Matriz m1(2, 2);
	Matriz m2(2, 2);

	m1.imprimir();
	m2.imprimir();

	Matriz m3(transposta(&m1));

	m3.imprimir();

	return 0;
}
