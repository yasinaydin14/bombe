#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
using namespace std;

int n, m;
vector<vector<int>> clauses;
vector<int> variables;
vector<int> trail;
vector<vector<int>> solutions;
bool stop_at_first = false;

bool read_cnf(const string& path)
{
    ifstream in(path);
    if (!in) return false;
    string tok;
    while (in >> tok) {
        if (tok == "c") { getline(in, tok); continue; }
        if (tok == "p") { in >> tok >> n >> m; break; }
    }
    if (!in) return false;
    vector<int> cur;
    int x;
    while ((int)clauses.size() < m && in >> x) {
        if (x == 0) { clauses.push_back(cur); cur.clear(); }
        else cur.push_back(x);
    }
    return (int)clauses.size() == m;
}

int lit_value(int lit){
	int v=variables[abs(lit)];
	if(v==-1) return -1;
	if(lit>0) return v;
	return 1-v;
}

void assign_lit(int lit){
	if(lit<0) variables[-lit]=0;
	if(lit>0) variables[lit]=1;
	trail.push_back(abs(lit));
}

bool unit_propogate(){
	bool changed=true;
	while(changed){
		changed=false;
		for(int i=0;i<(int)clauses.size();i++){
			int number_of_unassigned_literal=0;
			int unit_literal=0;
			bool satisfied=false;
			for(int j=0;j<(int)clauses[i].size();j++){
				if(lit_value(clauses[i][j])==1){ satisfied=true; break; }
				if(lit_value(clauses[i][j])==-1){ number_of_unassigned_literal++; unit_literal=clauses[i][j]; }
			}
			if(satisfied) continue;
			if(number_of_unassigned_literal==0) return false;
			if(number_of_unassigned_literal==1){ assign_lit(unit_literal); changed=true; }
		}
	}
	return true;
}

void undo(int mark){
	while((int)trail.size()>mark){ variables[trail.back()]=-1; trail.pop_back(); }
}

bool all_satisfied(){
	for(int i=0;i<(int)clauses.size();i++){
		bool s=false;
		for(int lit: clauses[i]) if(lit_value(lit)==1){ s=true; break; }
		if(!s) return false;
	}
	return true;
}

int pick_var(){
	for(int v=1;v<=n;v++) if(variables[v]==-1) return v;
	return 0;
}



void dpll(){
	int mark=trail.size();
	if(unit_propogate()==false){
		undo(mark);
		return;
	}
	int v=pick_var();
	if(v==0){
		if(all_satisfied()){
			solutions.push_back(variables);
		}
		undo(mark);
		return;
	}
	int after=trail.size();
	assign_lit(-v);
	dpll();
	undo(after);
	if(stop_at_first && !solutions.empty()){   
		undo(mark);
		return;
	}

	assign_lit(v);
	dpll();
	undo(mark);
}

bool write_solution(const string& path){
	ofstream out(path);
	if(!out) return false;

	if(solutions.empty()){
		out << "s UNSATISFIABLE\n";
		return true;
	}

	out << "s SATISFIABLE\n";
	for(auto& s : solutions){
		out << "v";
		for(int v=1; v<=n; v++){
			out << " " << (s[v]==1 ? v : -v);
		}
		out << " 0\n";
	}
	return true;
}


int main(int argc, char** argv)
{
    if(argc < 3){
        cerr << "kullanim: " << argv[0] << " girdi.cnf cikti.sol [all]\n";
        return 1;
    }
    stop_at_first = !(argc >= 4 && string(argv[3]) == "all");

    if(!read_cnf(argv[1])){
        cerr << "dosya okunamadi: " << argv[1] << "\n";
        return 1;
    }

    variables.assign(n + 1, -1);
    dpll();

    if(!write_solution(argv[2])){
        cerr << "cikti dosyasi acilamadi: " << argv[2] << "\n";
        return 1;
    }

    cout << (solutions.empty() ? "UNSAT" : "SAT")
         << ", " << solutions.size() << " cozum -> " << argv[2] << "\n";
    return 0;
}
