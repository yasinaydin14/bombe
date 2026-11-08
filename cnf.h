#ifndef CNF_H
#define CNF_H


#include "enigma.h"
#include "DSU.h"
#include <stdio.h>
#include <stdlib.h>

struct menu{
	char *crib;
	char *ciphertext;	
	struct enigma *enigma;
};

struct CNF{
	int n;
	int m;
	int capacity;
	int **clauses;
	int *length;
	int *class_var;
	int *var_class;
};

struct CNF make_cnf(void);

int add_clause(struct CNF *cnf);

void add_literal(struct CNF *cnf,int i,int lit);

struct DSU create_equivalence_classes(struct enigma* enigma, char* crib, char* plain, int n);

void free_cnf(struct CNF *cnf);

void write_dimacs(struct CNF *cnf, FILE *out);

void add_binary(struct CNF *cnf, int a, int b);

void add_at_least_one(struct DSU *d, struct CNF *cnf);

int pair_literal(struct DSU *d, struct CNF *cnf, char a, char x);

void assign_class_vars(struct DSU *d, struct CNF *cnf);

void add_at_most_one(struct DSU *d, struct CNF *cnf);

struct CNF dsu2cnf(struct DSU *d);



#endif
