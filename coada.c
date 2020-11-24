/*TOMA Andrei-311CB*/
#include "lib.h"

void *InitQ(int d)
{
	void *c = (AQ)malloc(sizeof(TCoada)); //alocare coada
	if (!c) return NULL; //verificare alocare
	((AQ)c)->dime = d; //setare dimensiunea unui element din coada
	// setare coada vida
	((AQ)c)->ic = NULL; 
	((AQ)c)->sc = NULL;
	return (void *)c;
}

int IntrQ(void *c, void *ae)
{
	ACelSt aux = (ACelSt)malloc(sizeof(TCelSt)); //alocare celula
	if (!aux) return 0; //verificare alocare
	aux->info = malloc(DIMEQ(c)); //alocare campul info cu dimensiunea cozii
	if (!aux->info) //verificare alocare
	{
		free(aux);
		return 0;
	}
	memcpy(aux->info, ae, DIMEQ(c)); //copiere in aux->info val de la adr ae
	aux->urm = NULL;
	// introudcere in coada
	if (VIDAQ(c))
	{
		IC(c) = aux;
		SC(c) = aux;
		return 1;
	}
	SC(c)->urm = aux;
	SC(c) = aux;
	return 1;
}

int ExtrQ(void *c, void *ae)
{
	if (VIDAQ(c)) return 0; //verificare coada vida
	ACelSt aux = IC(c);
	//actualizare inceput coada
	IC(c) = aux->urm;
	memcpy(ae, aux->info, DIMEQ(c)); //extragere din coada
	// eliberare memorie
	free(aux->info);
	free(aux);
	if (IC(c) == NULL)
		SC(c) = NULL;
	return 1;
}

void ResetQ(void *c, void (*Distr)(void *))
{
	ACelSt p;
	p = IC(c);
	IC(c) = NULL;
	SC(c) = NULL;
	DistrugeL(&p, Distr);
}

void DistrQ(void **c, void (*Distr)(void *))
{
	ResetQ(*c, Distr);
	free(*c);
}
