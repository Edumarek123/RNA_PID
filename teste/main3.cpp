#include <iostream>
#include "../libs/matriz_lib.hpp"


int main() {
	Matriz **MM;

	MM=(Matriz** )malloc(2*sizeof(Matriz*));

	MM[0]=new Matriz(2,1);

	for(int i=0;i<1;i++)
		delete MM[i];	

	free(MM);

	return 0;
}
