/*TOMA Andrei-311CB*/
#include <stdio.h>
#include <stdlib.h>

#ifndef _COADA_
#define _COADA_

typedef struct coada
{
	int dime; /*dimensiune element*/
	ACelSt ic, sc; /*adr. prima, ultima celula*/
} TCoada, *AQ;

#define DIMEQ(c) (((AQ)(c))->dime) /*dimensiunea elementelor din coada*/
#define IC(c) (((AQ)(c))->ic) /*inceputul cozii*/
#define SC(c) (((AQ)(c))->sc) /*sfarsitul cozii*/
#define VIDAQ(c) (((AQ)(c))->ic == NULL && ((AQ)c)->sc == NULL) /*coada vida*/

/*initializare coada cu dimensiunea d*/
void *InitQ(int d);

/*inserare in coada valoarea de la adresa ae alocata*/
int IntrQ(void *a, void *ae);

/*extragere din coada valoarea si o copiaza la adressa ae*/
int ExtrQ(void *a, void *ae);

/*distrugere coada a si mai primeste ca parametru un pointer la o functie care
distruge informatia din lista*/
void DistrQ(void **a, void (*Distr)(void *));

/*reseteaza coada (distruge lista)*/
void ResetQ(void *a, void (*Distr)(void *));

#endif
