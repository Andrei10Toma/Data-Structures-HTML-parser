/*Toma Andrei 311 CB*/
#include "lib.h"

/*
 *  Pentru a ușura citirea codului HTML din fișier, vi se pune la dispoziție
 *  următoarea funcție. Aceasta primeste un singur caracter (fișierul va fi
 *  citit caracter cu caracter în afara acestei funcții, iar ea va fi apelată
 *  de fiecare dată) și, în funcție de caracter, trece dintr-o stare în alta.
 *  Comentariile din cod arată o sugestie (neobligatorie) despre cum ar trebui
 *  completată funcția pentru a construi arborele. 
 * 
 *  Este, cu alte cuvinte, un automat de stări, unde starea inițială este
 * PARSE_CONTENTS.
 */

// cauta in arborele a nodul cu id-ul pid si il returneaza
TArb Parcurgere(TArb a, char *pid) {
    int ok = 0;
    TArb p_a = a; //cu p_a voi parcurge arborele
    if (strcmp(p_a->info->id, pid) == 0) {
        return p_a;
    }
    char *copie = (char *)calloc(sizeof(char), strlen(pid) + 1);
    char *aux = (char *)calloc(sizeof(char), strlen(pid) + 1);
    aux[0] = '\0'; copie[0] = '\0';
    char sep[] = ".";
    strncpy(copie, pid, strlen(pid));
    char *p = strtok(copie, sep); //cu p extrag pe rand fiecarui numar din pid
    strcat(aux, p); //in aux voi face append fiecarui numar din pid
    while (p) {
        if (p_a->firstChild) {
            //dupa ce gaseste aux-ul se duce pe primul copil al nodului
            p_a = p_a->firstChild;
        }
        else {
            ok = 1;
        }
        while (p_a) {
            //se duce pe fiecare frate pana gaseste aux-ul cautat
            if (strcmp(p_a->info->id, aux) == 0) {
                break;
            }
            if (p_a->nextSibling) {
                p_a = p_a->nextSibling;
            }
            else {
                //daca se ajunge pe ultimul frate si nu este identic cu aux
                //inseamna ca nodul cu id-ul pid nu a fost gasit si ret NULL
                if (strcmp(p_a->info->id, aux) == 0) {
                    break;
                }
                else {
                    free(copie); free(aux);
                    return NULL;
                }
            }
        }
        //extrage urmatorul numar din ip
        p = strtok(NULL, sep);
        if (p && ok == 0) {
            //actualizare aux
            strcat(aux, ".");
            strcat(aux, p);
        }
        else if (ok == 1) {
            //p_a este furnza
            p_a = NULL;
            break;
        }
    }
    free(copie);
    free(aux);
    return p_a;
}

//Sterge numarul dupa ultmul punct din pid pentru a obtine id-ul parintelui
void Sterge_Ultim_Punct(char *pid) {
    char *aux = NULL;
    aux = strrchr(pid, '.');
    if (aux) {
        pid[strlen(pid) - strlen(aux)] = '\0';
    }
    else {
        pid[0] = '\0';
        pid[0] = '0';
    }
}

void Sterge_Spatii(char *s) 
{
    int count = 0, i;
    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] != ' ') {
            s[count++] = s[i];
        }
    }
    s[count] = '\0'; 
}

// creaza un nou nod a si il leaga in arbore
void Create_Tag_Node(TArb a, char *pid, char c) {
    TArb aux, p_a;
    //daca radacina este goala inseamna ca este tagul <html>
    if (a->info->id[0] == '\0') {
        strcpy(a->info->id, "0");
        a->info->type[0] = c;
        strcpy(pid, a->info->id);
    }
    //daca radacina arborelui nu are copil
    else if (!a->firstChild) {
        //aloc nodul
        aux = Init_Arb();
        //append in type
        aux->info->type[0] = c;
        //si ii actualizez id-ul
        strcpy(aux->info->id, "1");
        strcpy(pid, aux->info->id);
        //legare in arbore
        a->firstChild = aux;
    }
    else {
        int nrChild = 0;
        aux = Init_Arb();
        aux->info->type[0] = c;
        //ne ducem pe nodul curent
        p_a = Parcurgere(a, pid);
        char *id_nou = (char *)malloc(sizeof(char) * MAX_C);
        if (!p_a->firstChild) {
            //actualizare id
            nrChild = 1;
            strcat(pid, ".");
            sprintf(id_nou, "%d", nrChild);
            strcat(pid, id_nou);
            strcpy(aux->info->id, pid);
            p_a->firstChild = aux;
        }
        else {
            TArb p_a_c = p_a->firstChild, aux2;
            //parcurgem totic copii si ii numaram
            while (p_a_c) {
                nrChild++;
                aux2 = p_a_c;
                p_a_c = p_a_c->nextSibling;
            }
            if (pid[0] == '0') {
                pid[0] = '\0';
            }
            else {
                strcat(pid, ".");
            }
            //adaugare id la noudl nou creat
            sprintf(id_nou, "%d", nrChild + 1);
            strcat(pid, id_nou);
            strcpy(aux->info->id, pid);
            aux2->nextSibling = aux;
        }
        free(id_nou);
    }
}

//adauga in listele style sau otherAttributes
void Add_Lists(TArb a, char *tmp_name, char *val_name) {
    char *copie_tmp_name = (char *)calloc(sizeof(char), MAX_C);
    char *copie_val_name = (char *)calloc(sizeof(char), MAX_C);
    strcpy(copie_tmp_name, tmp_name);
    strcpy(copie_val_name, val_name);
    Sterge_Spatii(copie_tmp_name);
    Sterge_Spatii(copie_val_name);
    if (strncmp(copie_tmp_name, "style", strlen("style")) == 0) {
        //adauga in lista style
        char *p1, *p2;
        char sep[] = ":; ";
        p1 = strtok(copie_val_name, sep);
        p2 = strtok(NULL, sep);
        //se extrag cate 2 striguri din copie_val_name care reprezinta numele,
        //respctiv valoarea atributului
        while (p1 && p2) {
            Adauga_Final_Lista(&a->info->style, p1, p2);
            p1 = strtok(NULL, sep);
            p2 = strtok(NULL, sep);
        }
    }
    else {
        //adauga in lista otherAttributes
        Adauga_Final_Lista(&a->info->otherAttributes, tmp_name, val_name);
    }
    //resetare tmp_name si val_name
    memset(tmp_name, 0, strlen(tmp_name));
    memset(val_name, 0, strlen(val_name));
    free(copie_tmp_name);
    free(copie_val_name);
}

TParseState Interpret(TParseState currentState, char c, TArb a, char *pid, 
    char *tmp_name, char *val_name)
{
    TParseState nextState = PARSE_ERROR;

    switch (currentState)
    {
    case PARSE_CONTENTS:
        if (c == '<')           {   nextState = PARSE_OPENING_BRACKET;  }
        else                    {   nextState = PARSE_CONTENTS;
                                    TArb a_aux = Parcurgere(a, pid);
                                    if (isalpha(c) || c == ' ' || isdigit(c) 
                                        || c == '.') {
                                        a_aux->info->contents
                                        [strlen(a_aux->info->contents)] = c;
                                        /*append in contents*/
                                    }
                                                                        }                             
        break;
    case PARSE_OPENING_BRACKET:
        if (isspace(c))         {   nextState = PARSE_OPENING_BRACKET;  }
        else if (c == '>')      {   nextState = PARSE_ERROR;            }
        else if (c == '/')      {   nextState = PARSE_CLOSING_TAG;      }
        else                    {   nextState = PARSE_TAG_TYPE;
                                    Create_Tag_Node(a, pid, c);
                                    /*Adauga nod in arborele a*/        }

        break;
    case PARSE_TAG_TYPE:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else                    {   nextState = PARSE_TAG_TYPE;
                                    TArb r = Parcurgere(a, pid);
                                    r->info->type[strlen(r->info->type)] = c;
                                    /*append in type*/                   }
        break;
    case PARSE_CLOSING_TAG:
        if (c == '>')           {   nextState = PARSE_CONTENTS;
                                    TArb a_aux = Parcurgere(a, pid);
                                    a_aux->info->isSelfClosing = 0;
                                    Sterge_Ultim_Punct(pid);            
                            /*se inchide tagul; se intoarce la parinte*/}
        else                    {   nextState = PARSE_CLOSING_TAG;      }
        break;
    case PARSE_REST_OF_TAG:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else if (c == '/')      {   nextState = PARSE_SELF_CLOSING;     }
        else                    {   nextState = PARSE_ATTRIBUTE_NAME;
                                    tmp_name[strlen(tmp_name)] = c;     }
        break;
    case PARSE_ATTRIBUTE_NAME:
        if (c == '=')           {   nextState = PARSE_ATTRIBUTE_EQ;     }
        else                    {   nextState = PARSE_ATTRIBUTE_NAME;
                                    tmp_name[strlen(tmp_name)] = c;
                                    /*append in numele atributului*/  }
        break;
    case PARSE_ATTRIBUTE_EQ:
        if (c == '\"')          {   nextState = PARSE_ATTRIBUTE_VALUE;  }
        break;
    case PARSE_ATTRIBUTE_VALUE:
        if (c == '\"')          {   nextState = PARSE_REST_OF_TAG;
                                    TArb a_aux = Parcurgere(a, pid);
                                    Add_Lists(a_aux, tmp_name, val_name);
                                                                        }
        else                    {   nextState = PARSE_ATTRIBUTE_VALUE;
                                    val_name[strlen(val_name)] = c;
                                    /*append in valoarea atributului*/  }
        break;
    case PARSE_SELF_CLOSING:
        if (c == '>')           {   nextState = PARSE_CONTENTS;
                                    TArb a_aux = Parcurgere(a, pid);
                                    a_aux->info->isSelfClosing = 1;
                                    Sterge_Ultim_Punct(pid);
                            /*se inchide tagul; se intoarce la parinte*/}
        else                    {   nextState = PARSE_ERROR;            }
        break;
    default:
        break;
    }
    return nextState;
}