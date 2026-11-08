#include <stdio.h>
#include <stdlib.h>
#include "DSU.h"

struct DSU make_dsu(){
	struct DSU d;
	d.length = 0;
	d.capacity= 4;
	d.pc = malloc(4*sizeof(struct plugboard_connection));
	d.size= malloc(4*sizeof(int));
	d.parent= malloc(4*sizeof(int));
	d.classes= malloc(4*sizeof(int));

	return d;	
}

void free_dsu(struct DSU *d){
	free(d->pc);
	free(d->parent);
	free(d->classes);
	free(d->size);
	d->length=0;
	d->capacity=0;
}

int make_set(struct DSU* dsu, char letter1, char letter2){
	int index=dsu->length;
	struct plugboard_connection pc;
	int existing=element(dsu,letter1,letter2);
	
	if(existing!=-1)
		return existing;

	if(dsu->length==dsu->capacity){
		int new_capacity=2*dsu->capacity;
		struct plugboard_connection *new_pc;
		int *new_size, *new_parent, *new_classes;

		new_pc=realloc(dsu->pc,new_capacity*sizeof(struct plugboard_connection));
		if(new_pc==NULL) return -1;
		dsu->pc=new_pc;

		new_size=realloc(dsu->size,new_capacity*sizeof(int));
		if(new_size==NULL) return -1;
		dsu->size=new_size;

		new_parent=realloc(dsu->parent,new_capacity*sizeof(int));
		if(new_parent==NULL) return -1;
		dsu->parent=new_parent;

		new_classes=realloc(dsu->classes,new_capacity*sizeof(int));
		if(new_classes==NULL) return -1;
		dsu->classes=new_classes;

		dsu->capacity=new_capacity;
	}

	pc.letter1=letter1;
	pc.letter2=letter2;
	pc.truth_value=1;
	dsu->pc[index]=pc;
	dsu->parent[index]=index;
	dsu->size[index]=1;
	dsu->classes[index]=index;
	(dsu->length)++;

	return index;
}

int find(struct DSU *d, int i){
	if(i==d->parent[i]) 
		return i;
	return d->parent[i]= find(d, d->parent[i]);
	

}

void unite(struct DSU *d, int i, int j){
	int a= find(d,i);
	int b= find(d,j);
	if(a!=b){
		if(d->size[a] < d->size[b]){
			int next;
			if(truth_value(d,a)==0){
				d->pc[b].truth_value=0;
			}
			if(truth_value(d,b)!=0){
				int x=b;
				int flag=0;
				do{
					int y= a;
					do{
						struct plugboard_connection p1=d->pc[x];
						struct plugboard_connection p2=d->pc[y];
						if((p1.letter1==p2.letter1 && p1.letter2!=p2.letter2) || (p1.letter1==p2.letter2 && p1.letter2!=p2.letter1) || (p2.letter1==p1.letter2 && p2.letter2!=p1.letter1)  || (p1.letter2==p2.letter2 && p1.letter1!=p2.letter1) ){
							printf("Contradiction detected!!\n");
							d->pc[b].truth_value=0;
							flag=1;
							break;
						
						}
						
						y=d->classes[y];
						
					}
					while(y!=a);
					if(flag) break;
					x=d->classes[x];
				} 
				while(x!=b);
			}
					
			d->parent[a]=b;
			d->size[b]+=d->size[a];
			next=d->classes[a];
			d->classes[a]=d->classes[b];
			d->classes[b]=next;
			
			
		}

		else {
			int next;
			if(truth_value(d,b)==0){
				d->pc[a].truth_value=0;
			}

			if(truth_value(d,a)!=0){
                                int x=b;
                                int flag=0;
                                do{
                                        int y=a;
                                        do{
                                                struct plugboard_connection p1=d->pc[x];
                                                struct plugboard_connection p2=d->pc[y];
                                                if((p1.letter1==p2.letter1 && p1.letter2!=p2.letter2) || (p1.letter1==p2.letter2 && p1.letter2!=p2.letter1) || (p2.letter1==p1.letter2 && p2.letter2!=p1.letter1)  || (p1.letter2==p2.letter2 && p1.letter1!=p2.letter1) ){
                                                        printf("Contradiction detected!!\n");
							d->pc[a].truth_value=0;
                                                        flag=1;
                                                        break;

                                                }

                                                y=d->classes[y];

                                        }
					while(y!=a);
                                        if(flag) break;
                                        x=d->classes[x];
                                }
				while(x!=b);
                        }


			d->parent[b]=a;
			d->size[a]+=d->size[b];
			next=d->classes[a];
                        d->classes[a]=d->classes[b];
                        d->classes[b]=next;

		}
	}
}

int size(struct DSU *d, int i){
	int a = find(d,i);
	return d->size[a];
}

int element(struct DSU *d, char letter1, char letter2){
	int i;
	for(i=0;i<d->length;i++){
		if((d->pc[i].letter1==letter1 && d->pc[i].letter2==letter2) || (d->pc[i].letter2==letter1 && d->pc[i].letter1==letter2)){
			return i;		
		} 
			
	}
	return -1;
}

int equivalent(struct DSU *d, int i, int j){
	return find(d,i)==find(d,j);
}

void mark(struct DSU* d, int i, int b){
	if(b!=0 && b!=1){
		printf("Invalid mark value!!\n");
		return;
	} 
		
	int a= find(d,i);
	d->pc[a].truth_value=b;
}

int truth_value(struct DSU* d, int i){
	int a = find(d,i);
	return d->pc[a].truth_value;
}
/*
int main(){
	struct DSU d=make_dsu() ;
	int i;
	make_set(&d,'A', 'B');
	make_set(&d, 'C','D');
	make_set(&d,'F','E');
	make_set(&d,'A','G');
	for(i=0;i<d.length;i++){
		printf("%c is connected to %c \n",d.pc[i].letter1, d.pc[i].letter2);
		
	}
	unite(&d, 0,1);
	unite(&d,0,2);
	unite(&d,1,3);
	printf("Size: %i\n",size(&d,1));
	printf("Letter %c and Letter %c are matched in %i\n",'C', 'D', element(&d, 'C','D'));
	printf("Index %i and index %i are %i\n",0,1,equivalent(&d,0,1));
	mark(&d,0,2);
	printf("Truth value of index 0 is %i\n",truth_value(&d,1));
	return 0;
}*/
