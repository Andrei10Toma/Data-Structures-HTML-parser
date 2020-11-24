/*TOMA Andrei-311CB*/
#include "lista_generica.h"

void DistrugeL(ACelSt *l, void(*freeinfo)(void *))
{
	ACelSt p;
	for (p = *l;p != NULL;)
	{
		//distruge fiecare element din lista cu ajutorul functei freeinfo
		//freeinfo elibereaza memoria din campul info
		ACelSt aux = p;
		p = p->urm;
		freeinfo(aux->info);
		free(aux);
	}
}

