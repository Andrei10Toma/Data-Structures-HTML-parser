/*Toma Andrei 311 CB*/
#include "lib.h"

TArb Init_Arb() {
	TArb a = (TArb)calloc(sizeof(TNodArb), 1);
	if (!a) return NULL;
	a->info = (TInfo)calloc(sizeof(TNodInfo), 1);
	if (!a->info) {
		free(a);
		return NULL;
	}
	a->info->contents = (char *)calloc(sizeof(char), MAX_C);
	if (!a->info->contents) {
		free(a->info);
		free(a);
		return NULL;
	}
	a->info->id = (char *)calloc(sizeof(char), MAX_C);
	if (!a->info->id) {
		free(a->info->contents);
		free(a->info);
		free(a);
		return NULL;
	}
	a->info->type = (char *)calloc(sizeof(char), MAX_C);
	if (!a->info->type) {
		free(a->info->id);
		free(a->info->contents);
		free(a->info);
		free(a);
		return NULL;
	}
	return a;
}
  
TAttr Aloca_Cel(char *name, char *value) {
	TAttr cel;
	name[strlen(name) ] = '\0';
	value[strlen(value) ] = '\0';
	cel = (TAttr)malloc(sizeof(TNodAttr));
	if (!cel) {
		return NULL;
	}
	cel->next = NULL;
	cel->name = (char *)calloc(sizeof(char), strlen(name) + 10);
	if (!cel->name) {
		free(cel);
		return NULL;
	}
	cel->value = (char *)calloc(sizeof(char), strlen(value) + 10);
	if (!cel->value) {
		free(cel->name);
		free(cel);
		return NULL;
	}
	strncpy(cel->name, name, strlen(name));
	strncpy(cel->value, value, strlen(value));
	return cel;
}

int Adauga_Final_Lista(TAttr *l, char *name, char *value) {
	TAttr p, ultim = NULL, add = Aloca_Cel(name, value);
	//determinare ultim element din lista
	for (p = (*l); p != NULL; ultim = p, p = p->next);
	if (ultim == NULL) {
		(*l) = add;
	}
	else {
		ultim->next = add;
	}
	return 1;
}

void Distruge_Lista(TAttr *l) {
	TAttr p, aux;
	for (p = (*l); p != NULL;) {
		aux = p;
		p = p->next;
		free(aux->name);
		free(aux->value);
		free(aux);
	}
	(*l) = NULL;
}

void Format(TArb a, int n, FILE *out) {
	if (!a) {
		return;
	}
	int i;
	TArb p; TAttr p1;
	for (p = a; p != NULL; p = p->nextSibling) {
		for (i = 0; i < n; i++) {
			fprintf(out, "\t");
		}
		if (!p->info->style && !p->info->otherAttributes) {
			//daca listele sunt goale
			fprintf(out, "<%s>\n", p->info->type);
		}
		else {
			fprintf(out, "<%s ", p->info->type);
			//afiseaza elementele din style
			if (p->info->style) {
				fprintf(out, "style=\"");
				for (p1 = p->info->style; p1 != NULL; p1 = p1->next) {
					fprintf(out, "%s: %s;", p1->name, p1->value);
					if (p1->next) {
						fprintf(out, " ");
					}
				}
				fprintf(out, "\"");
			}
			//afiseaza lista otherAttributes
			if (p->info->otherAttributes) {
				if (p->info->style) {
					fprintf(out, " ");
				}
				for (p1 = p->info->otherAttributes; p1 != NULL; p1 = p1->next) {
					fprintf(out, "%s=\"%s\"", p1->name, p1->value);
					if (p1->next) {
						fprintf(out, " ");
					}
				}
			}
			//modalitatea de inchidere a tagului
			if (p->info->isSelfClosing == 1){
				fprintf(out, "/>\n");
			}
			else {
				fprintf(out, ">\n");
			}
		}
		// afiseaza contentul
		if (p->info->contents[0] != '\0') {
			for (i = 0; i < n + 1; i++) {
				fprintf(out, "\t");
			}
			fprintf(out, "%s\n", p->info->contents);
		}
		//intra in recursivitate pentru fiecare copil al lui p
		//se mareste numarul de taburi
		Format(p->firstChild, n+1, out);
		//dupa iesirea din recursivitatea se inchid tagurile care nu sunt 
		//self closing
		if (p->info->isSelfClosing != 1) {
			for (i = 0; i < n; i++) {
				fprintf(out, "\t");
			}
			fprintf(out, "</%s>\n", p->info->type);
		}
	}
}

int Adauga_Tag(TArb a, char *id, char *tag, FILE *out) {
	Sterge_Spatii(tag);
	//cauta nodul cu id-ul id
	TArb aux = Parcurgere(a, id);
	if (!aux) {
		fprintf(out, "Add tag failed: node with id %s not found!\n", id);
		return 0;
	}
	char *tmp_name = (char *)calloc(sizeof(char), MAX_C);
	char *val_name = (char *)calloc(sizeof(char), MAX_C);
	unsigned int state = PARSE_CONTENTS, i;
	//adaugare tag cu ajutorul functiei Interpret
	for (i = 0; i < strlen(tag); i++) {
		state = Interpret(state, tag[i], a, id, tmp_name, val_name);
	}
	free(tmp_name);
	free(val_name);
	return 1;
}

void Actualizare_ID(TArb a, int nrCopil) {
	if (!a) {
		return;
	}
	nrCopil = 0;
	TArb p;
	char id1[10];
	//actualizaeaza id-urile fiecarui frate
	for (p = a->firstChild; p != NULL; p = p->nextSibling) {
		nrCopil++;
		memset(p->info->id, 0, strlen(p->info->id));
		strcat(p->info->id, a->info->id);
		strcat(p->info->id, ".");
		sprintf(id1, "%d", nrCopil);
		strcat(p->info->id, id1);
		Actualizare_ID(p, nrCopil);
	}
	//intra in recursivitate pentru fiecare copil
	Actualizare_ID(a->firstChild, 0);
}

void Delete_Recursively_aux(TArb a, TArb x) {
	char *pid = (char *)calloc(sizeof(char), MAX_C);
	strcpy(pid, x->info->id);
	Sterge_Ultim_Punct(pid);
	TArb parent = Parcurgere(a, pid), eliminat;
	TArb p_a_s, ant;
	//daca nodul care trebuie sa fie eliminat este primul copil al parintelui
	if (strcmp(parent->firstChild->info->id, x->info->id) == 0) {
		eliminat = parent->firstChild;
		parent->firstChild = x->nextSibling;
		Distruge_Arb(&eliminat);
	}
	else {
		for (p_a_s = parent->firstChild; p_a_s != NULL; ) {
			//cauta in arbore nodul care trebuie sa fie eliminat
			if (strcmp(p_a_s->info->id, x->info->id) == 0) {
				eliminat = p_a_s;
				p_a_s = p_a_s->nextSibling;
				//refacere legaturi
				if (ant != NULL) {
					ant->nextSibling = x->nextSibling;
				}
				else {
					parent->firstChild->nextSibling = x->nextSibling;
				}
				//distruge tot arborele de la nodul care trebuie eliminat
				Distruge_Arb(&eliminat);
			}
			else {
				//incrementare pas
				ant = p_a_s;
				p_a_s = p_a_s->nextSibling;
			}
		}
	}
	free(pid);
	Actualizare_ID(parent, 0);
}

void Delete_Recursively_4(TArb a, char *tag, char *class, FILE *out, 
	char *selector) {
	//coada folosita pentru parcrugerea in latime
	void *c = InitQ(sizeof(TNodArb)); 
	TArb p_a, x = (TArb)malloc(sizeof(TNodArb));
	TAttr p;
	int ok = 0;
	IntrQ(c, a);
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp(x->info->type, tag) == 0) {
			for (p = x->info->otherAttributes; p != NULL; p = p->next) {
				if (strcmp(p->name, "class") == 0 && 
					strcmp(p->value, class) == 0) {
					//un nod care respecta selectorul este gasit
					ok = 1;
					Delete_Recursively_aux(a, x);
					break;
				}
			}
		}
		else {
			for (p_a = x->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
				IntrQ(c, p_a);
			}
		}
	}
	if (ok == 0) {
		fprintf(out,
			"Delete recursively failed: no node found for selector %s!\n", 
			selector);
	}
	DistrQ(&c, FreeInfo);
	free(x);
}

void Delete_Recursively_3(TArb a, char *tag, FILE *out) {
	void *c = InitQ(sizeof(TNodArb));
	TArb p_a, x = (TArb)malloc(sizeof(TNodArb));
	int ok = 0;
	IntrQ(c, a);
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp(x->info->type, tag) == 0) {
			//este gasit un nod care respecta selectorul dat
			ok = 1;
			Delete_Recursively_aux(a, x);
		}
		else {
			//daca nu este sters copii lui sunt introdusi in coada
			for (p_a = x->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
				IntrQ(c, p_a);
			}
		}
	}
	if (ok == 0) {
		fprintf(out,
			"Delete revursively failed: no node found for selector %s!\n", tag);
	}
	DistrQ(&c, FreeInfo);
	free(x);
}


void Delete_Recursively_1(TArb a, char *id, char *selector, FILE *out) {
	//se duce pe nodul cu id-ul id
	TArb arb = Parcurgere(a, id);
	if (!arb) {
		fprintf(out, 
			"Delete revursively failed: no node found for selector %s!\n", 
			selector);
		return;
	}
	//daca este gasit este sters
	Delete_Recursively_aux(a, arb);
}

void Delete_Recursively_6(TArb a, char *tag1, char *tag2, char *selector, 
	FILE *out) {
	//coada pentru parcurgerea in latime a arborelui
	void *c = InitQ(sizeof(TNodArb));
	TArb p_a, x = (TArb)malloc(sizeof(TNodArb));
	int ok = 0;
	IntrQ(c, a);
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp(x->info->type, tag1) == 0) {
			//coda aux pentru parcurgerea in latime a arborelui din nodul x
			//pentru a trece prin toti urmasii lui
			void *c_aux = InitQ(sizeof(TNodArb));
			TArb p_a_aux, x_aux = (TArb)malloc(sizeof(TNodArb));
			IntrQ(c_aux, x);
			while (!VIDAQ(c_aux)) {
				ExtrQ(c_aux, x_aux);
				if (strcmp(x_aux->info->type, tag2) == 0) {
					//nodul care respecta selectorul cu id-ul 6 este gasit
					ok = 1;
					Delete_Recursively_aux(a, x_aux);
				}
				else {
					//daca nu este gasit copii sunt introdusi in coada aux
					for (p_a_aux = x_aux->firstChild; p_a_aux != NULL; 
						p_a_aux = p_a_aux->nextSibling) {
						IntrQ(c_aux, p_a_aux);
					}
				}	
			}
			DistrQ(&c_aux, FreeInfo);
			free(x_aux);
		}
		for (p_a = x->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, p_a);
		}
	}
	if (ok == 0) {
		fprintf(out, 
			"Delete revursively failed: no node found for selector %s!\n",
			selector);
	}
	DistrQ(&c, FreeInfo);
	free(x);
}

void Delete_Recursively_5(TArb a, char *tag1, char *tag2, 
	char *selector, FILE *out) {
	void *c = InitQ(sizeof(TNodArb));
	TArb p_a, x = (TArb)malloc(sizeof(TNodArb)), p_a_s;
	int ok = 0, ok_aux = 0;
	IntrQ(c, a);
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		ok_aux = 0;
		if (strcmp(x->info->type, tag1) == 0) {
			//cauta in copii nodului cu tag 1
			for (p_a_s = x->firstChild; p_a_s != NULL; 
				p_a_s = p_a_s->nextSibling) {
				if (strcmp(p_a_s->info->type, tag2) == 0) {
					//nod care respecta selectorul cu id-ul 5 este gasit
					ok_aux = 1;
					ok = 1;
					Delete_Recursively_aux(a, p_a_s);
					break;
				}
			}
		}
		if (ok_aux == 0) {
			//daca nu este gasit niciun nod copii sunt introdusi in coada
			for (p_a = x->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
				IntrQ(c, p_a);
			}
		}
	}
	if (ok == 0) {
		fprintf(out, 
			"Delete recursively failed: no node found for selector %s!\n", 
			selector);
	}
	DistrQ(&c, FreeInfo);
	free(x);
}

void Delete_Recursively(TArb a, int id_selector, char *selector, FILE *out) {
	char *p1, *p2;
	char *copie_selector = (char *)calloc(sizeof(char), MAX_C);
	selector[strlen(selector) - 1] = '\0';
	strcpy(copie_selector, selector);
	//apelare functie pentru selectorul dat ca parametru
	if (id_selector == -2) {
		free(copie_selector);
		return;
	}
	if (id_selector == 4) {
		p1 = strtok(selector, ".");
		p2 = strtok(NULL, ".");
		Delete_Recursively_4(a, p1, p2, out, copie_selector);
	}
	if (id_selector == 3) {
		Delete_Recursively_3(a, selector, out);
	}
	if (id_selector == 1) {
		selector = selector + 1;
		Delete_Recursively_1(a, selector, copie_selector, out);
	}
	if (id_selector == 6) {
		p1 = strtok(selector, " ");
		p2 = strtok(NULL, " ");
		Delete_Recursively_6(a, p1, p2, copie_selector, out);
	}
	if (id_selector == 5) {
		char *p1 = strtok(selector, ">");
		char *p2 = strtok(NULL, ">");
		Delete_Recursively_5(a, p1, p2, copie_selector, out);
	}
	free(copie_selector);
}

void Append_Style_aux(TArb a, char *style) {
	char *style_copie = (char *)calloc(sizeof(char), MAX_C);
	style_copie[0] = '\0';
	strncpy(style_copie, style, strlen(style));
	char *tok1 = NULL, *tok2 = NULL;
	//extragere nume
	tok1 = strtok(style_copie, ":; ");
	//extragere valoare
	tok2 = strtok(NULL, ":; ");
	TAttr p;
	while(tok1 && tok2) {
		for (p = a->info->style; p != NULL; p = p->next) {
			if (strcmp(p->name, tok1) == 0) {
				//daca numele este deja in lista de style
				//actualizare valoare
				memset(p->value, 0, strlen(p->value));
				strncpy(p->value, tok2, strlen(tok2));
				break;
			}
		}
		if (p == NULL) {
			//daca nu este gasit numele, numele si valoarea sunt adaugate la
			//finalul listei style
			Adauga_Final_Lista(&a->info->style, tok1, tok2);
		}
		tok1 = strtok(NULL, ":; ");
		tok2 = strtok(NULL, ":; ");
	}
	free(style_copie);
}

void Append_Style_3(TArb a, char *tag, char *style, FILE *out) {
	//coada care contine adresele pointerilor catre noduri
	void *c = InitQ(sizeof(TNodArb*));
	TArb p_a, *x = (TArb*)malloc(sizeof(TNodArb*));
	IntrQ(c, &a);
	int ok = 0;
	while(!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp((*x)->info->type, tag) == 0) {
			//este gasit nod care respecta selectorul cu id 3
			ok = 1;
			Append_Style_aux(*x, style);
		}
		//introducere copii lui *x in coada
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
	}
	if (ok == 0) {
		fprintf(out, "Append to style failed: no node found for selector %s!\n"
			, tag);
	}
	free(x);
	DistrQ(&c, FreeInfo);
}

void Append_Style_4(TArb a, char *selector, char *style, char *tag, 
	char *class, FILE *out) {
	void *c = InitQ(sizeof(TNodArb*));
	TArb p_a, *x = (TArb*)malloc(sizeof(TNodArb*));
	IntrQ(c, &a);
	int ok = 0;
	TAttr poA;
	while(!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp((*x)->info->type, tag) == 0) {
			for (poA = (*x)->info->otherAttributes; poA != NULL; 
				poA = poA->next) {
				if (strcmp(poA->name, "class") == 0 && 
					strcmp(poA->value, class) == 0) {
					//este gasit nodul care respecta selectorul cu id-ul 4
					ok = 1;
					Append_Style_aux(*x, style);
				}
			}
		}
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
	}
	if (ok == 0) {
		fprintf(out, "Append to style failed: no node found for selector %s!\n"
			, selector);
	}
	free(x);
	DistrQ(&c, FreeInfo);
}


void Append_Style_1(TArb a, char *id, char *style, char *selector, FILE *out) {
	//este cautat nodul care are id-ul id
	TArb nod = Parcurgere(a, id);
	if (nod == NULL) {
		fprintf(out,
			"Append to style failed: no node found for selector %s!\n", 
			selector);
		return;
	}
	Append_Style_aux(nod, style);
}

void Append_Style_2 (TArb a, char *class, char *selector, char *style,
	FILE *out) {
	void *c = InitQ(sizeof(TNodArb*));
	TArb p_a, *x = (TArb*)malloc(sizeof(TNodArb*));
	IntrQ(c, &a);
	int ok = 0;
	TAttr poA;
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		for (poA = (*x)->info->otherAttributes; poA != NULL; poA = poA->next) {
			if (strcmp(poA->name, "class") == 0 
				&& strcmp(poA->value, class) == 0) {
				//nodul care respecta selectorul cu id-ul 2 este gasit
				ok = 1;
				Append_Style_aux(*x, style);
			}	
		}
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
	}
	if (ok == 0) {
		fprintf(out,
			"Append to style failed: no node found for selector %s!\n",
			selector);
	}
	free(x);
	DistrQ(&c, FreeInfo);
}

void Append_Style_6(TArb a, char *selector, char *style, char *tag1, 
	char *tag2, FILE *out) {
	void *c = InitQ(sizeof(TNodArb*));
	TArb p_a, *x = (TArb*)malloc(sizeof(TNodArb*));
	IntrQ(c, &a);
	int ok = 0;
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp((*x)->info->type, tag1) == 0) {
			//este gasit nodul cu tag1
			void *c_aux = InitQ(sizeof(TNodArb*));
			TArb p_a_aux, *x_aux = (TArb*)malloc(sizeof(TNodArb*));
			IntrQ(c_aux, x);
			while (!VIDAQ(c_aux)) {
				//sunt parcurse toate nodurile din nodul x pentru a gasi urmasii
				//care respecta selectoruls
				ExtrQ(c_aux, x_aux);
				if (strcmp((*x_aux)->info->type, tag2) == 0) {
					//este gasit nod care respecta selectorul dat
					ok = 1;
					Append_Style_aux(*x_aux, style);
				}
				for (p_a_aux = (*x_aux)->firstChild; p_a_aux != NULL; 
					p_a_aux = p_a_aux->nextSibling) {
					IntrQ(c_aux, &p_a_aux);
				}
			}
			DistrQ(&c_aux, FreeInfo);
			free(x_aux);
		}
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
	}
	if (ok == 0) {
		fprintf(out, 
			"Append to style failed: no node found for selector %s!\n", 
			selector);
	}
	free(x);
	DistrQ(&c, FreeInfo);
}

void Append_Style_5(TArb a, char *selector, char *tag1, char *tag2, 
	char *style, FILE *out) {
	void *c = InitQ(sizeof(TNodArb*));
	TArb p_a, *x = (TArb *)malloc(sizeof(TNodArb*)), p_a_s;
	IntrQ(c, &a);
	int ok = 0;
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp((*x)->info->type, tag1) == 0) {
			//cautare copii lui *x care respecta selectorul
			for (p_a_s = (*x)->firstChild; p_a_s != NULL; 
				p_a_s = p_a_s->nextSibling) {
				if (strcmp(p_a_s->info->type, tag2) == 0) {
					ok = 1;
					Append_Style_aux(p_a_s, style);
				}
			}
		} 
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
	}
	if (ok == 0) {
		fprintf(out, 
			"Append to style failed: no node found for selector %s!\n", 
			selector);	
	}
	free(x);
	DistrQ(&c, FreeInfo);
}

void Append_Style(TArb a, int id_selector, char *selector, 
	char *style, FILE *out) {
	char *copie_selector = (char *)calloc(sizeof(char), MAX_C);
	Sterge_Spatii(style);
	strcpy(copie_selector, selector);
	//apelare functie pentru selectorul dat ca parametru
	if (id_selector == 3) {
		Append_Style_3(a, selector, style, out);
		free(copie_selector);
		return;
	}
	if (id_selector == 4) {
		char *p1 = strtok(selector, ".");
		char *p2 = strtok(NULL, ".");
		Append_Style_4(a, copie_selector, style, p1, p2, out);
		free(copie_selector);
		return;
	}
	if (id_selector == 2) {
		char *class = selector + 1;
		Append_Style_2(a, class, copie_selector, style, out);
		free(copie_selector);
		return;
	}
	if (id_selector == 1) {
		char *id = selector + 1;
		Append_Style_1(a, id, style, copie_selector, out);
		free(copie_selector);
		return;
	}
	if (id_selector == 6) {
		char *p1 = strtok(selector, " ");
		char *p2 = strtok(NULL, " ");
		Append_Style_6(a, copie_selector, style, p1, p2, out);
		free(copie_selector);
		return;
	}
	if (id_selector == 5) {
		char *p1 = strtok(selector, ">");
		char *p2 = strtok(NULL, " ");
		Append_Style_5(a, copie_selector, p1, p2, style, out);
		free(copie_selector);
		return;
	}
	free(copie_selector);
}

void Override_Style_aux(TArb a, char *style) {
	Distruge_Lista(&a->info->style);
	char *style_copie = (char *)malloc(sizeof(char) * MAX_C);
	if (!style_copie) {
		return;
	}
	style_copie[0] = '\0';
	strcpy(style_copie, style);
	//extragere nume atribut
	char *tok1 = strtok(style_copie, ":; ");
	//extragere valoare atribut
	char *tok2 = strtok(NULL, ":; ");
	while (tok1 && tok2) {
		Adauga_Final_Lista(&a->info->style, tok1, tok2);
		tok1 = strtok(NULL, ":; ");
		tok2 = strtok(NULL, ":; ");
	}
	free(style_copie);
}

void Override_Style_3(TArb a, char *tag, char *style, FILE *out) {
	//coada care contine adresa pointerilor catre nodurile arborelui
	void *c = InitQ(sizeof(TNodArb*));
	TArb p_a, *x = (TArb *)malloc(sizeof(TNodArb*));
	IntrQ(c, &a);
	int ok = 0;
	while(!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp((*x)->info->type, tag) == 0) {
			//este gasit nodul care respecta conditia
			ok = 1;
			Override_Style_aux(*x, style);
		}
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
	}
	if (ok == 0) {
		fprintf(out, 
			"Override style failed: no node found for selector %s!\n", tag);
	}
	free(x);
	DistrQ(&c, FreeInfo);
}

void Override_Style_4(TArb a, char *selector, char *style, char *tag, 
	char *class, FILE *out) {
	void *c = InitQ(sizeof(TNodArb*));
	TArb p_a, *x = (TArb*)malloc(sizeof(TNodArb*));
	IntrQ(c, &a);
	int ok = 0;
	TAttr p;
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp((*x)->info->type, tag) == 0) {
			//parcurge lista de other attributes
			for (p = (*x)->info->otherAttributes; p != NULL; p = p->next) {
				if (strcmp(p->name, "class") == 0 &&
					strcmp(p->value, class) == 0) {
					//este gasit nodul care respecta selectorul
					ok = 1;
					Override_Style_aux(*x, style);
					break;
				}
			}
		}
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
	}
	if (ok == 0) {
		fprintf(out, 
			"Override style failed: no node found for selector %s\n", 
			selector);
	}
	DistrQ(&c, FreeInfo);
	free(x);
}

void Override_Style_5(TArb a, char *tag1, char *tag2, char *selector, 
	char *style, FILE *out) {
	void *c = InitQ(sizeof(TNodArb*));
	TArb p_a, *x = (TArb *)malloc(sizeof(TNodArb*)), p;
	IntrQ(c, &a);
	int ok = 0;
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp((*x)->info->type, tag1) == 0) {
			//sunt parcursi toti copii nodului *x
			for (p = (*x)->firstChild; p != NULL; p = p->nextSibling) {
				if (strcmp(p->info->type, tag2) == 0) {
					//selectorul este respectat
					ok = 1;
					Override_Style_aux(p, style);
					break;
				}
			}
		}
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
	}
	if (ok == 0) {
		fprintf(out, 
			"Override style failed: no node found for selector %s\n", 
			selector);	
	}
	DistrQ(&c, FreeInfo);
	free(x);
}

void Override_Style_6(TArb a, char *selector, char *style, char *tag1, 
	char *tag2, FILE *out) {
	void *c = InitQ(sizeof(TNodArb*));
	TArb p_a, *x = (TArb *)malloc(sizeof(TNodArb*));
	IntrQ(c, &a);
	int ok = 0;
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		if (strcmp((*x)->info->type, tag1) == 0) {
			void *c_aux = InitQ(sizeof(TNodArb*));
			TArb p_a_aux, *x_aux = (TArb *)malloc(sizeof(TNodArb*));
			IntrQ(c_aux, x);
			while (!VIDAQ(c_aux)) {
				//sunt parcursi toti urmasii lui x
				ExtrQ(c_aux, x_aux);
				if (strcmp((*x_aux)->info->type, tag2) == 0) {
					//selectorul este respectat
					ok = 1;
					Override_Style_aux(*x_aux, style);
				}
				for (p_a_aux = (*x_aux)->firstChild; p_a_aux != NULL; 
					p_a_aux = p_a_aux->nextSibling) {
					IntrQ(c_aux, &p_a_aux);
				}
			}
			DistrQ(&c_aux, FreeInfo);
			free(x_aux);
		}
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
	}
	if (ok == 0) {
		fprintf(out, 
			"Override style failed: no node found for selector %s!\n", 
			selector);
	}
	DistrQ(&c, FreeInfo);
	free(x);
}

void Override_Style(TArb a, int id_selector, char *selector, 
	char *style, FILE *out) {
	Sterge_Spatii(style);
	char *copie_selector = (char *)calloc(sizeof(char), MAX_C);
	copie_selector[0] = '\0';
	strcpy(copie_selector, selector);
	//apelare functie pentru fiecare selector
	if (id_selector == 3) {
		Override_Style_3(a, copie_selector, style, out);
		free(copie_selector);
		return;
	}
	if (id_selector == 4) {
		char *p1 = strtok(selector, ".");
		char *p2 = strtok(NULL, ".");
		Override_Style_4(a, copie_selector, style, p1, p2, out);
		free(copie_selector);
		return;
	}
	if (id_selector == 5) {
		char *p1 = strtok(selector, ">");
		char *p2 = strtok(NULL, ">");
		Override_Style_5(a, p1, p2, copie_selector, style, out);
		free(copie_selector);
		return;
	}
	if (id_selector == 6) {
		char *p1 = strtok(selector, " ");
		char *p2 = strtok(NULL, " ");
		Override_Style_6(a, copie_selector, style, p1, p2, out);
		free(copie_selector);
		return;
	}
}

void FreeInfo(void* a) {
	Distruge_Lista(&((TArb)a)->info->style);
	Distruge_Lista(&((TArb)a)->info->otherAttributes);
	free(((TArb)a)->info->id);
	free(((TArb)a)->info->type);
	free(((TArb)a)->info->contents);
	free(((TArb)a)->info);
	free(((TArb)a));
}

void Distruge_Arb(TArb *a) {
	void *c = InitQ(sizeof(TNodArb*));
	TArb *x = (TArb*)malloc(sizeof(TNodArb*)), p_a;
	IntrQ(c, a);
	while (!VIDAQ(c)) {
		ExtrQ(c, x);
		for (p_a = (*x)->firstChild; p_a != NULL; p_a = p_a->nextSibling) {
			IntrQ(c, &p_a);
		}
		FreeInfo(*x);
	}
	free(x);
	DistrQ(&c, FreeInfo);
}
