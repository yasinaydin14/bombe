#include <stdio.h>
#include <stdlib.h>
#include "DSU.h"
#include "cnf.h"

int passed=0, failed=0;

void check(const char *name, int ok){
	if(ok){ printf("  OK    %s\n",name); passed++; }
	else  { printf("  HATA  %s\n",name); failed++; }
}

/* 351 ciftin hepsini sabit sirayla acar: AA, AB, ..., AZ, BB, ..., ZZ */
void make_all_pairs(struct DSU *d){
	int a,b;
	for(a=0;a<26;a++)
		for(b=a;b<26;b++)
			make_set(d,'A'+a,'A'+b);
}

int main(void){
	struct CNF cnf;
	struct DSU d;
	int i, k, ok, v, x_abcd;

	/* ---------- 1) CNF temel fonksiyonlari ---------- */
	printf("CNF temel fonksiyonlari\n");
	cnf=make_cnf();
	cnf.n=5;
	for(i=0;i<10;i++){                      /* 10 cumle: kapasite 4 -> 8 -> 16 */
		k=add_clause(&cnf);
		check(i==0 ? "add_clause ilk indeks 0" : "add_clause sirali indeks", k==i);
		add_literal(&cnf,k,i%5+1);
		add_literal(&cnf,k,-((i+1)%5+1));
	}
	add_binary(&cnf,3,-4);
	k=add_clause(&cnf);                      /* bos cumle */
	check("m == 12", cnf.m==12);
	check("kapasite buyudu (>= 12)", cnf.capacity>=12);
	check("cumle 0 = [1, -2]", cnf.length[0]==2 && cnf.clauses[0][0]==1 && cnf.clauses[0][1]==-2);
	check("cumle 9 = [5, -1]", cnf.length[9]==2 && cnf.clauses[9][0]==5 && cnf.clauses[9][1]==-1);
	check("add_binary = [3, -4]", cnf.length[10]==2 && cnf.clauses[10][0]==3 && cnf.clauses[10][1]==-4);
	check("bos cumle uzunlugu 0", cnf.length[11]==0);
	printf("  write_dimacs ciktisi:\n");
	write_dimacs(&cnf,stdout);
	free_cnf(&cnf);

	/* ---------- 2) Hic birlesme yok: 351 tekli sinif ---------- */
	printf("\nBirlesme yok (351 tekli sinif)\n");
	d=make_dsu();
	make_all_pairs(&d);
	check("DSU'da 351 cift", d.length==351);
	cnf=make_cnf();
	assign_class_vars(&d,&cnf);
	check("n == 351", cnf.n==351);
	ok=1;
	for(v=1;v<=cnf.n;v++) if(cnf.class_var[cnf.var_class[v]]!=v) ok=0;
	check("var_class, class_var'in tersi", ok);
	check("pair_literal(A,B) == pair_literal(B,A)", pair_literal(&d,&cnf,'A','B')==pair_literal(&d,&cnf,'B','A'));
	check("pair_literal(A,B) != 0", pair_literal(&d,&cnf,'A','B')!=0);
	add_at_least_one(&d,&cnf);
	check("at least one: 26 cumle", cnf.m==26);
	ok=1;
	for(i=0;i<26;i++) if(cnf.length[i]!=26) ok=0;
	check("at least one: her cumle 26 literal", ok);
	add_at_most_one(&d,&cnf);
	check("at most one: 26 * 325 = 8450 cumle", cnf.m-26==8450);
	ok=1;
	for(i=26;i<cnf.m;i++) if(cnf.length[i]!=2 || cnf.clauses[i][0]>=0 || cnf.clauses[i][1]>=0) ok=0;
	check("at most one: hepsi iki negatif literal", ok);
	free_cnf(&cnf);
	free_dsu(&d);

	/* ---------- 3) Birlesme ve celiski ---------- */
	printf("\nBirlesme: {AB, CD} tutarli, {AF, AG} celiskili\n");
	d=make_dsu();
	make_all_pairs(&d);
	unite(&d,element(&d,'A','B'),element(&d,'C','D'));
	unite(&d,element(&d,'A','F'),element(&d,'A','G'));     /* A iki ese bagli -> celiski */
	check("DSU'da hala 351 cift", d.length==351);
	cnf=make_cnf();
	assign_class_vars(&d,&cnf);
	check("n == 348 (351 - 1 birlesme - 1 birlesme - 1 celiskili sinif)", cnf.n==348);
	x_abcd=pair_literal(&d,&cnf,'A','B');
	check("AB ve CD ayni degisken", x_abcd!=0 && x_abcd==pair_literal(&d,&cnf,'C','D'));
	check("DC de ayni degisken (sira onemsiz)", x_abcd==pair_literal(&d,&cnf,'D','C'));
	check("AF imkansiz (0)", pair_literal(&d,&cnf,'A','F')==0);
	check("AG imkansiz (0)", pair_literal(&d,&cnf,'G','A')==0);
	ok=1;
	for(v=1;v<=cnf.n;v++) if(cnf.class_var[cnf.var_class[v]]!=v) ok=0;
	check("var_class, class_var'in tersi", ok);

	add_at_least_one(&d,&cnf);
	check("at least one: 26 cumle", cnf.m==26);
	check("A'nin cumlesi 24 literal (AF, AG yok)", cnf.length[0]==24);
	check("B'nin cumlesi 26 literal", cnf.length[1]==26);
	check("F'nin cumlesi 25 literal (AF yok)", cnf.length[5]==25);
	check("G'nin cumlesi 25 literal (AG yok)", cnf.length[6]==25);
	ok=0;
	for(i=0;i<cnf.length[1];i++) if(cnf.clauses[1][i]==x_abcd) ok=1;
	check("B'nin cumlesinde {AB,CD} degiskeni var", ok);

	k=cnf.m;
	add_at_most_one(&d,&cnf);
	/* 276 + 2*300 + 23*325 = 8351, ayni cumleden 4 kopya ayiklaniyor */
	check("at most one: 8347 cumle (tekrarlar ayiklandi)", cnf.m-k==8347);
	ok=1;
	for(i=k;i<cnf.m;i++) if(cnf.length[i]!=2 || cnf.clauses[i][0]>=0 || cnf.clauses[i][1]>=0) ok=0;
	check("at most one: hepsi iki negatif literal", ok);

	{
		FILE *f=fopen("test_dsu.cnf","w");
		if(f!=NULL){ write_dimacs(&cnf,f); fclose(f); printf("  test_dsu.cnf yazildi\n"); }
	}
	free_cnf(&cnf);
	free_dsu(&d);

	printf("\n%d gecti, %d basarisiz\n",passed,failed);
	return failed==0 ? 0 : 1;
}
