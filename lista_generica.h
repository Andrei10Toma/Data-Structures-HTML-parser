/*TOMA Andrei-311CB*/
#include <stdio.h>
#include <stdlib.h>

#ifndef _LISTA_
#define _LISTA_

typedef struct celst
{
	struct celst *urm;
	void *info;
} TCelSt, *ACelSt;

/*distruge lista generica*/
void DistrugeL(ACelSt *l, void(*freeInfo)(void *));

#endif
