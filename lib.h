/*Toma Andrei 311 CB*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lista_generica.h"
#include "coada.h"

#define MAX_C 1024*10

typedef struct TNodAttr {
	char *name;
	char *value;
	struct TNodAttr *next;
} TNodAttr, *TAttr;

typedef struct TNodInfo {
	char *type;
	char *id;
	TAttr style;
	TAttr otherAttributes;
	int isSelfClosing;
	char *contents;
} TNodInfo, *TInfo;

typedef struct TNodArb {
	TInfo info;
	struct TNodArb *nextSibling;
	struct TNodArb *firstChild;
} TNodArb, *TArb;

typedef enum
{
	PARSE_ERROR             = 0,
	PARSE_CONTENTS          = 1,
	PARSE_OPENING_BRACKET   = 2,
	PARSE_TAG_TYPE          = 3,
	PARSE_CLOSING_TAG       = 4,
	PARSE_REST_OF_TAG       = 5,
	PARSE_ATTRIBUTE_NAME    = 6,
	PARSE_ATTRIBUTE_EQ      = 7,
	PARSE_ATTRIBUTE_VALUE   = 8,
	PARSE_SELF_CLOSING      = 9,
} TParseState;

TParseState Interpret(TParseState currentState, char c, TArb a, char *pid, 
	char *tmp_name, char *val_name);
/*
 * functia parseaza un fisier html cu ajutorul unui automat de stari;
 * currentState = starea curenta a automatului;
 * c = caracterul citit din fisierul html;
 * a = arborele creat pana acum;
 * pid = id-ul ultimului nod adaugat;
 * tmp_name = se retine numele atributului;
 * val_name = se retine valoarea atributului.
 */

TArb Parcurgere(TArb a, char *pid);
/*
 * functia parcurge in mod eficient dupa id arborele si returneaza nodul cu
 * id-ul pid, daca nu il gaseste returneaza NULL;
 * a = radacina arborelui creat;
 * pid = id-ul care va fi cautat in arborele cu radacina a. 
 */

void Sterge_Ultim_Punct(char *pid);
/*
 * am folosit aceasta functie pentru a obtine id-ul parintelui unui nod;
 * pid = id-ul din care se va sterge partea dupa ultimul punct.
 */

void Sterge_Spatii(char *s);
/*
 * am folosit aceasta functie pentru a elimina spatiile din valoriile listei
 * style si din tagurile de la comanda add;
 * s = sirul de caractere dat pentru a elimina spatiile din el.
 */

void Create_Tag_Node(TArb a, char *pid, char c);
/*
 * functia creaza un nou nod, il leaga in arbore si ii afla id-ul;
 * a = radacina arborelui;
 * pid = id-ul nodului curent;
 * c = caracterul citit din fisierul html.
 */

void Add_Lists(TArb a, char *tmp_name, char *val_name);
/* 
 * functia adauga ori in lista de style ori in cea de otherAttributes in 
 * functie de tmp_name;
 * a = radacina arborelui a;
 * tmp_name = numele atributului;
 * val_name = valoarea atributului respectiv.
 */

TArb Init_Arb();
/*
 * functia aloca memorie pentru un nod.
 */

TAttr Aloca_Cel(char *name, char *value);
/*
 * aloca o celula pentru una dintre cele 2 liste si copiaza in campurile name
 * si value valorile date ca parametru;
 * name = numele atributului;
 * value = valoarea atributului.
 */

int Adauga_Final_Lista(TAttr *l, char *name, char *value);
/*
 * aceasta functie aloca o celula si o adauga la finalul listei l;
 * l = lista (otherAttributes sau style) in care dorim sa adaugam celula cu
 * numele name si valoarea value;
 * name = numele atributului;
 * value = valoarea atributului.
 */

void Distruge_Lista(TAttr *l);
/*
 * functia distruge lista *l.
 */

void Format (TArb a, int n, FILE *out);
/*
 * functia afiseaza codul html parsat in a in fisierul out;
 * a = arborele care contine fisierul html parsat;
 * n = reprezinta numarul de taburi;
 * out = fisierul in care este afisat html-ul.
 */

int Adauga_Tag(TArb a, char *id, char *tag, FILE *out);
/*
 * functia adauga un tag nou in arborele a ca ultim copil al nodului cu id-ul
 * id;
 * a = radacina arborelui;
 * id = id-ul nodului parinte la care vrem sa adaugam ca ultim copil tagul tag;
 * tag = tagul pe care dorim sa il adaugam in arbore;
 * out = fisierul in care se afiseaza mesajul de eroare in cazul in care nu se
 * gaseste nodul cu id-ul id.
 */

void Actualizare_ID(TArb a, int nrCopil);
/*
 * functia actualizeaza id-urile recursiv in arborele care pleaca din nodul a; 
 * dupa stergerea unor noduri;
 * a = nodul din care sa inceapa actualizarea;
 * nrCopil = numarul copiilor al nodului a.
 */

/*
 * #ID = selectorul cu id-ul 1 (tag-ul cu id-ul id);
 * .className = selectorul cu id-ul 2 (tag-urile care au clasa className);
 * p = selectorul cu id-ul 3 (toate tag-urile <p>);
 * p.className = selectorul cu id-ul 4 (toate tag-urile <p> si class className);
 * tag1>tag2 = selectorul cu id-ul 5 (toate tag-urile <tag2> care au ca parinte
 * tag1);
 * tag1 tag2 = selectorul cu id-ul 6 (toate tag-urile tag2 care au ca stramos 
 * un tag tag1);
 */

void Delete_Recursively(TArb a, int id_selector, char *selector, FILE *out);
/*
 * functia este apelata cand este intalnita comanda deleteRecursively si va
 * sterge recursiv toate nodurile care respecta selectorul dat. Pentru fiecare
 * selector se va apela alta functie Delete_Recursively_{id_selector}
 * in functie de id-ul selectorului;
 * a = radacina arborelui din care se sterge;
 * id_selector = tipul selectorului;
 * selector = este selectorul propiru-zis;
 * out = fisierul in care se afiseaza mesajele de eroare. 
 */

void Delete_Recursively_aux(TArb a, TArb x);
/*
 * functia sterge si elibereaza memoria pentru nodul x si copii lui si reface
 * legaturile in arbore;
 * a = radacina arborelui a
 * x = nodul care trebuie eliminat din arbore
 */

void Append_Style(TArb a, int id_selector, char *selector, char *style, 
	FILE *out);
/*
 * functia este apelata cand este intalnita comanda appendStyle si va adauga sau
 * actualiza lista style pentru nodurile care respecta selectorul dat ca
 * parametru. Pentru fiecare selector se va apela o functie Append_Style_
 * {id_selector};
 * a = radacina arborelui;
 * id_selector = tipul selectorului;
 * selector = selectorul propriu-zis;
 * out = fisierul in care se afiseaza mesajul de eroare.
 */

void Append_Style_aux(TArb a, char *style);
/*
 * functia va face append in lista de style. Se extrag cate 2 valori din style
 * cu strtok care reprezinta numele atributului din style si valoarea 
 * atributului respectiv. Daca numele atributului deja exita in lista de style
 * acesta este actualizat cu valoarea. Altfel atributul este introdus in lista
 * de style cu nume si valoare.
 * a = nodul la care trebuie sa facem append style
 * style = sirul de cractere din care extragem numele si valoarea
 */

void Override_Style(TArb a, int id_selector, char *selector, char *style, 
	FILE *out);
/*
 * functia este apelata cand este intalnita comanda overrideStyle si va ditruge
 * lista de style pentru nodurile care respecta selectorul dat ca paramentru
 * si in lista style vor fi introduse noile nume si valori de atribute date ca
 * parametru in style. Pentru fiecare selector se va folosi o functie
 * Override_Style_{id_selector}. 
 * a = radacina arborelui;
 * id_selector = tipul selectorului:
 * selector = selectorul propriu-zis;
 * style = numele si valorile atributelor pe care trebuie sa le bagam in lista
 * de style dupa stergerea celei anterioare;
 * out = fisierul in care se afiseaza mesajele de eroare daca exista.
 */

void Override_Style_aux(TArb a, char *style);
/*
 * functia va face override in lista de style. Va distruge lista initiala de
 * style si o va introduce pe cea data ca parametru. Se extrag cate 2 valori
 * care reprezinta numele si valoarea atributului si sunt introduse in lista 
 * goala de style;
 * a = nodul in care trebuie sa facem override;
 * style = sirul de caractere cu noile atribute care trebuie adaugate in lista
 * de style.
 */

void Distruge_Arb(TArb *a);
/*
 * functia elibereaza memoria pentru tot arborele care porneste din nodul *a
 * prin parcurgerea in latime a arborelui din nodul dat;
 * a = adresa nodului din care se elibereaza memoria. 
 */

void FreeInfo(void *arb);
/*
 * elibereaza toata informatia dintr-un nod;
 * arb = nodul din care este eliberata informatia.
 */