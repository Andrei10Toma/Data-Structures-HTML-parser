/*Toma Andrei 311 CB*/
#include "lib.h"

//numara numarul de cifre ale unui intreg
int Numar_Cifre_Int(int x) {
	int contor = 0;
	if (x < 10) {
		return 1;
	}
	while (x != 0) {
		contor++;
		x /= 10;
	}
	return contor;
}

//extrage id-ul pentru comanda add
char *Gaseste_ID(char *cmd) {
	char *p = strtok(cmd, " ");
	p = strtok(NULL, " ");
	char *r = strchr(p, '=');
	r = r + 1;
	return r;
}

//extrage tag-ul pentru comanda add
char *Get_Tag(char *cmd) {
	char *r = strchr(cmd, '\"');
	r = r + 1;
	r[strlen(r) - 1] = '\0';
	return r;
}

//extrage selectorul pentru comanda deleteRecursively
char *Get_Selector(char *cmd) {
	char *selector = strstr(cmd, "selector=");
	selector = selector + strlen("selector=");
	selector = selector + 1;
	return selector;
}

//extrage style-ul pentru comenzile appendStyle si overrideStyle
char *Get_Style(char *cmd) {
	char *style = strstr(cmd, "style=");
	style = style + strlen("style=");
	style = style + 1;
	style[strlen(style) - 1] = '\0';
	return style;
}

//extrage id-ul (tipul) selectorului
int ID_Selector(char *cmd) {
	char *aux1, *aux2, *aux3, *aux4;
	char *selector = Get_Selector(cmd);
	aux1 = strchr(selector, '#');
	aux2 = strchr(selector, '.');
	aux3 = strchr(selector, '>');
	aux4 = strchr(selector, ' ');
	if (aux1) {
		return 1;
	}
	if (selector[0] == '.') {
		return 2;
	}
	if (!aux1 && !aux2 && !aux3 && !aux4) {
		return 3;
	}
	if (aux2 && selector[0] != '.') {
		return 4;
	}
	if (aux3) {
		return 5;
	}
	if (aux4) {
		return 6;
	}
	return -2;
}

//extrage selectorul pentru comanda append si override
void Get_Selector_Append(char *cmd, char **rez) {
	char *copie = (char *)calloc(sizeof(char), MAX_C);
	strcpy(copie, cmd);
	char *selector = NULL;
	selector = strstr(copie, "selector=");
	char *test = strchr(selector, ' ');
	char *p = (char *)calloc(sizeof(char), MAX_C);
	char *inc = p;
	p[0] = '\0';
	if (test[1] == 's' && test[2] == 't' && test[3] == 'y' && test[4] == 'l' 
		&& test[5] == 'e') {
		strcpy(p, strtok(selector, " "));
		p = p + strlen("selector=");
		p = p + 1;
		p[strlen(p) - 1] = '\0';
	}
	else {
		selector = selector + strlen("selector=");
		char *p1 = strtok(selector, " ");
		char *p2 = strtok(NULL, " ");
		strcat(p, p1);
		strcat(p, " ");
		strcat(p, p2);
		p = p + 1;
		p[strlen(p) - 1] = '\0';
	}
	free(copie);
	strcpy(*rez, p);
	free(inc);
	// return p;
}

//extrage id-ul (tipul) selectorului pentru comanda append si override
int ID_Selector_Append(char *cmd) {
	char *p;
	p = (char *)calloc(sizeof(char), MAX_C);
	Get_Selector_Append(cmd, &p);
	char *aux1, *aux2, *aux3, *aux4;
	aux1 = strchr(p, '#');
	aux2 = strchr(p, '.');
	aux3 = strchr(p, '>');
	aux4 = strchr(p, ' ');
	if (aux1) {
		free(p);
		return 1;
	}
	if (p[0] == '.') {
		free(p);
		return 2;
	}
	if (!aux1 && !aux2 && !aux3 && !aux4) {
		free(p);
		return 3;
	}
	if (aux2) {
		free(p);
		return 4;
	}
	if (aux3) {
		free(p);
		return 5;
	}
	if (aux4) {
		free(p);
		return 6;
	}
	free(p);
	return -2;
}

int main(int argc, char *argv[])
{
	TArb a = Init_Arb();
	int state = PARSE_CONTENTS, N, i;
	char *tmp_name = (char *)calloc(sizeof(char), MAX_C);
	if (!tmp_name)
		return -1;
	char *val_name = (char *)calloc(sizeof(char), MAX_C);
	if (!val_name) {
		free(tmp_name); return -1;
	}
	char *cmd = (char *)calloc(sizeof(char), MAX_C);
	if (!cmd) {
		free(val_name); free(tmp_name); return -1;
	}
	tmp_name[0] = '\0';
	val_name[0] = '\0';
	FILE *in_html = fopen(argv[argc - 3], "rt");
	if (!in_html) {
		return -1;
	}
	FILE *in_cmd = fopen(argv[argc - 2], "r");
	if (!in_cmd) {
		return -1;
	}
	FILE *out = fopen(argv[argc - 1], "wt");
	if (!out) {
		return -1;
	}
	char c, *pid;
	pid = (char *)calloc(sizeof(char), MAX_C);
	//citire din fisier caracter cu caracter
	while (1) {
		c = fgetc(in_html);
		if (feof(in_html)) {
			break;
		}
		if (c != '\n' || c != '\r') {
			state = Interpret(state, c, a, pid, tmp_name, val_name);
		}
	}
	fscanf(in_cmd, "%d", &N); //citire numar de comenzi
	//deplasare cursor din fisier pentru a putea citi comenzile
	fseek(in_cmd, Numar_Cifre_Int(N) + 1, SEEK_SET);
	for (i = 0; i < N; i++) {
		fgets(cmd, MAX_C, in_cmd);
		cmd[strlen(cmd) - 1] = '\0';
		char copie_cmd[strlen(cmd)]; strcpy(copie_cmd, cmd);
		if (strncmp(cmd, "add", strlen("add")) == 0) {
			Adauga_Tag(a, Gaseste_ID(copie_cmd), Get_Tag(cmd), out);
		}
		if (strncmp(cmd, "format", strlen("format")) == 0) {
			Format(a, 0, out);
		}
		if (strncmp(cmd, "deleteRecursively", 
			strlen("deleteRecursively")) == 0) {
			Delete_Recursively(a, ID_Selector(cmd), Get_Selector(cmd), out);
		}
		if (strncmp(cmd, "appendStyle", strlen("appendStyle")) == 0) {
			char *rez = (char *)calloc(sizeof(char), MAX_C);
			Get_Selector_Append(cmd, &rez);
			Append_Style(a, ID_Selector_Append(cmd), rez, Get_Style(cmd), out);
			free(rez);
		}
		if (strncmp(cmd, "overrideStyle", strlen("overrideStyle")) == 0) {
			char *rez = (char *)calloc(sizeof(char), MAX_C);
			Get_Selector_Append(cmd, &rez);
			Override_Style(a, ID_Selector_Append(cmd), rez, 
				Get_Style(cmd), out);
			free(rez);
		}
	}
	fclose(in_html); fclose(in_cmd); fclose(out);
	free(tmp_name); free(val_name); free(cmd); free(pid);
	Distruge_Arb(&a);
	return 0;
}