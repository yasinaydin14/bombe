#include "enigma.h"
#include "DSU.h"
#include "cnf.h"
#include <stdio.h>
#include <stdlib.h>

struct DSU create_equivalence_classes(struct enigma *enigma,char* crib, char* plain, int n){
	int i;
	struct enigma e = *enigma;
	struct DSU dsu = make_dsu();
	
	for(i=0;i<n;i++){
		char permutation[27];
		char current_letter;
		rotate_rotors(&e);
		rotor_and_reflector_permutation(&e, permutation );
		for(current_letter='A';current_letter<='Z';current_letter++){
			int j=make_set(&dsu, crib[i],current_letter);
			int k=make_set(&dsu, permutation[current_letter-'A'], plain[i]);
			unite(&dsu, j,k);
			
			
		}
		
	}
	
	return dsu;

}



struct CNF make_cnf(void){
	struct CNF c;
	int i;
	c.clauses=malloc(4*sizeof(int*));
	c.length=malloc(4*sizeof(int));
	c.capacity=4;
	for(i=0;i<4;i++){
		c.length[i]=0;
		c.clauses[i]=NULL;
	}
	c.n=0;
	c.m=0;
	c.var_class=NULL;
	c.class_var=NULL;
	
	return c;
}

void free_cnf(struct CNF *cnf){
	int i;
	for(i=0;i<cnf->m;i++){
		free(cnf->clauses[i]);
	}
	free(cnf->length);
	free(cnf->clauses);
	free(cnf->class_var);
	free(cnf->var_class);
	cnf->capacity=0;
	cnf->n=0;
	cnf->m=0;
}

int add_clause(struct CNF *cnf){
	if(cnf->m==cnf->capacity){
		cnf->capacity=2*cnf->capacity;
		cnf->clauses=realloc(cnf->clauses,cnf->capacity*sizeof(int*));
		cnf->length=realloc(cnf->length,cnf->capacity*sizeof(int));
	}

	cnf->clauses[cnf->m]=NULL;
	cnf->length[cnf->m]=0;
	(cnf->m)++;
	return cnf->m-1;
}

void add_literal(struct CNF *cnf, int i, int lit){
	cnf->clauses[i]=realloc(cnf->clauses[i],(cnf->length[i]+1)*sizeof(int));
	cnf->clauses[i][cnf->length[i]]=lit;
	(cnf->length[i])++;
}

void add_binary(struct CNF *cnf, int a, int b){
	int i=add_clause(cnf);
	add_literal(cnf, i,a);
	add_literal(cnf,i,b);
}

void write_dimacs(struct CNF *cnf, FILE *out){
	int i;
	int j;
	fprintf(out,"p cnf %i %i\n",cnf->n,cnf->m);
	for(i=0;i<cnf->m;i++){
		for(j=0;j<cnf->length[i];j++){
			fprintf(out,"%i ",cnf->clauses[i][j]);
		}
		fprintf(out,"0\n");
	}
}

void assign_class_vars(struct DSU *d, struct CNF *cnf){
	int n=0;
	int i;
	cnf->class_var = calloc(d->length, sizeof(int)); 
	cnf->var_class= calloc(d->length+1, sizeof(int)); 
	for(i=0;i<d->length;i++){
		int r= find(d,i);
		if(truth_value(d,r) && cnf->class_var[r]==0){
			n++;
			cnf->class_var[r]=n;
			cnf->var_class[n]=r;
		}
	}
	cnf->n=n;
}


int pair_literal(struct DSU *d, struct CNF *cnf, char a, char x){
	int idx = element(d,a,x);
	if (idx==-1)
		return 0;
	return cnf->class_var[find(d,idx)];
}

void add_at_least_one(struct DSU *d, struct CNF *cnf){
	char i,j;
	for(i='A';i<='Z';i++){
		int c= add_clause(cnf);
		for(j='A';j<='Z';j++){
			int v= pair_literal(d,cnf,i,j);
			if(v!=0){
				add_literal(cnf,c,v);
			}
		}
	}
}

void add_at_most_one(struct DSU *d, struct CNF *cnf){
	char* seen = calloc((cnf->n + 1) * (cnf->n + 1), sizeof(char));
	char i,j,k;
	for(k='A';k<='Z';k++){
		for(i='A';i<='Z';i++){
			for(j=i+1;j<='Z';j++){
				int v1,v2;
				v1=pair_literal(d,cnf,k,i);
				v2=pair_literal(d,cnf,k,j);
				if(v1 && v2 && seen[v1*(cnf->n+1)+v2]==0){
					seen[v1*(cnf->n+1)+v2]='1';
					seen[v2*(cnf->n+1)+v1]='1';
					add_binary(cnf,-v1,-v2);
				}
			}
		}
	}
	free(seen);
}

struct CNF dsu2cnf(struct DSU *d){
	struct CNF cnf= make_cnf();
	assign_class_vars(d, &cnf);
	add_at_most_one(d,&cnf);
	add_at_least_one(d,&cnf);
	return cnf;
	
}

int read_solution(const char *path, int n, int *values){
	FILE *f;
	char word[32];
	int lit, i;

	f=fopen(path,"r");
	if(f==NULL)
		return -1;

	if(fscanf(f,"%31s",word)!=1 || strcmp(word,"s")!=0 || fscanf(f,"%31s",word)!=1){
		fclose(f);
		return -1;
	}
	if(strcmp(word,"SATISFIABLE")!=0){
		fclose(f);
		return 0;
	}

	for(i=0;i<=n;i++)
		values[i]=0;

	if(fscanf(f,"%31s",word)!=1 || strcmp(word,"v")!=0){
		fclose(f);
		return -1;
	}
	while(fscanf(f,"%d",&lit)==1 && lit!=0){
		if(abs(lit)<=n)
			values[abs(lit)]= lit>0 ? 1 : 0;
	}
	fclose(f);
	return 1;
}