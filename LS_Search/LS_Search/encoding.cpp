#include "encoding.h"

vector<int> transform_to_binary(int u) {
	int k = 0;
	int t = 1;
	vector<int> degrees;
	vector<int> res;
	
	for (int i = 0; t < u; i++) {
		degrees.push_back(t);
		res.push_back(0);
		t = t * 2;
		k++;
		
	}
	int v = u;
	for (int i = 0; i < k; i++)
	{
		res[i] = v / degrees[k - i - 1];
		v = v - res[i] * degrees[k - i - 1];
	}
	return res;
	//need to check if k is correct when u=2^l
}

void ALO(B_Cell t, vector<vector<int>> &E) {
	int k = 1;
	for (int i = 0; i < t.variables.size(); i++) {
		k = k * 2;
	}
	vector<vector<int>> clauses;
	for (int i = t.maxvalue; i < k; i++) {
		vector<int> br = transform_to_binary(i);
		
		vector<int> clause;
		for (int j = 0; j < br.size(); j++) {
			if (br[j] == 1)
			{
				clause.push_back(-t.variables[j]);
			}
			else {
				clause.push_back(t.variables[j]);
			}		
		}
		clauses.push_back(clause);
	}
	//debug
	/*for (int i = 0; i < clauses.size(); i++) {
		cout << endl;
		for (int j = 0; j < clauses[i].size(); j++) {
			cout << clauses[i][j] << " ";
		}
	}
	*/
	bool b = true;
	if (clauses.size()>0){
		for (int i = 0; i < clauses.size() - 1; i++) {
			vector<int> t1 = clauses[i];
			for (int j = i + 1; j < clauses.size();j++){
				vector<int> t2 = clauses[i+1];			
				if (t1.size() == t2.size()) {
					bool b = true;
					for (int k = 0; k < t1.size() - 1; k++) {
						if (t1[k] != t2[k]) {
							b = false;
							break;
						}
					}
					if ((b == true) && (t1.back() + t2.back() == 0)) {
						clauses[i].erase(clauses[i].begin() + clauses[i].size() - 1);
						clauses.erase(clauses.begin() + i + 1);
						i = 0;
						j = clauses.size();
					}
				}		
			}
		}
	}
	/*
	cout << endl << "After resolving" << endl;
	for (int i = 0; i < clauses.size(); i++) {
		cout << endl;
		for (int j = 0; j < clauses[i].size(); j++) {
			cout << clauses[i][j] << " ";
		}
	}
	*/


	for (int i = 0; i < clauses.size(); i++) {
		E.push_back(clauses[i]);
	}
}

void newvareqequiv(int lp, int rp1, int rp2, vector<vector<int>>&E) {
	vector<int> smc1;
	vector<int> smc2;
	vector<int> smc3;
	vector<int> smc4;

	smc1.push_back(lp);
	smc2.push_back(lp);
	smc3.push_back(-lp);
	smc4.push_back(-lp);

	smc1.push_back(rp1);
	smc2.push_back(-rp1);
	smc3.push_back(rp1);
	smc4.push_back(-rp1);

	smc1.push_back(rp2);
	smc2.push_back(-rp2);
	smc3.push_back(-rp2);
	smc4.push_back(rp2);

	E.push_back(smc1);
	E.push_back(smc2);
	E.push_back(smc3);
	E.push_back(smc4);
}

void ineq(B_Cell A, B_Cell B, int &nv, vector<vector<int>>& E) {
	vector<int> aux_vars;
	vector<int> clause;
	for (int i = 0; i < A.variables.size(); i++) {
		aux_vars.push_back(nv++);
		newvareqequiv(aux_vars[i], A.variables[i], B.variables[i], E);
		clause.push_back(-aux_vars[i]);
	}
	E.push_back(clause);
}


void ALO(vector<int>t, vector<vector<int>> &E) {
	vector<int> bigclause;
	for (int i = 0; i < t.size(); i++) {
		bigclause.push_back(t[i]);
	}
	E.push_back(bigclause);
}

void ALO(C_Cell t, vector<vector<int>> & E) {
	ALO(t.variables, E);
}

void ALO(vector<C_Cell> t, vector<vector<int>> & E) {
	//here we suppose that it is either row or column
	int dim = t[0].variables.size();
	for (int i = 0; i < dim; i++) {
		vector<int> u;
		for (int j = 0; j < t.size(); j++) {
			u.push_back(t[j].variables[i]);
		}
		ALO(u, E);
	}
}


void AMO_pairwise(vector<int>t, vector<vector<int>> &E) {
	for (int i = 0; i < t.size(); i++) {
		for (int j = i + 1; j < t.size(); j++) {
			vector<int> smallclause;
			smallclause.push_back(-t[i]);
			smallclause.push_back(-t[j]);
			E.push_back(smallclause);
		}
	}
}

vector<int> bin_pp(vector<int> c) {
	vector<int> r;
	for (int i = 0; i < c.size(); i++) {
		r.push_back(c[i]);
	}
	int carry = 1;
	for (int i = 0; i < r.size(); i++) {
		if ((r[i] == 0) && (carry == 1)) {
			r[i] = 1;
			carry = 0;
		}
		if ((r[i] == 1) && (carry == 1)) {
			r[i] = 0;
		}
	}
	return r;
}

vector<vector<int>> Bin_Rep(int d) {//binary representation
	vector<vector<int>> r;
	int t = 1;
	int u = 0;
	while (t < d) {
		t = t * 2;
		u++;
	}
	vector<int> seed;
	for (int i = 0; i < u; i++) {
		seed.push_back(0);
	}
	r.push_back(seed);
	for (int i = 1; i < d; i++) {
		r.push_back(bin_pp(r.back()));
	}
	return r;
}

void AMO_Binary(vector<int>t, int & newvar, vector<vector<int>> &E) {
	vector<vector<int>> br = Bin_Rep(t.size());
	int u = br[0].size();
	vector<int> aux_vars;

	for (int i = 0; i < u; i++) {
		aux_vars.push_back(newvar++);
	}

	for (int i = 0; i < t.size(); i++) {
		for (int j = 0; j < u; j++) {
			vector<int> smallclause;
			smallclause.push_back(-t[i]);
			if (br[i][j] == 0) {
				smallclause.push_back(-aux_vars[j]);
			}
			else {
				smallclause.push_back(aux_vars[j]);
			}
			E.push_back(smallclause);
		}
	}
}

void AMO_Commander(vector<int> t, int m, int & newvar, vector<vector<int>> &E) {
	//m = number of sets
	vector<vector<int>> sets;
	for (int i = 0; i < m; i++) {
		vector<int> s_i;
		sets.push_back(s_i);
	}

	int k = 0;
	for (int i = 0; i < t.size(); i++) {
		sets[k].push_back(t[i]);
		k++;
		if (k == m) { k = 0; }
	}
	vector<int> com_vars;

	for (int i = 0; i < m; i++) {
		sets[i].push_back(-(newvar++));
		com_vars.push_back(-sets[i].back());
		ALO(sets[i], E);
		AMO_pairwise(sets[i], E);
	}
	AMO_pairwise(com_vars, E);
}

void AMO_Product(vector<int> t, int p, int q, int & newvar, vector<vector<int>> &E) {
	vector<int> U;
	vector<int> V;
	for (int i = 0; i < p; i++) {
		U.push_back(newvar++);
	}

	for (int i = 0; i < q; i++) {
		V.push_back(newvar++);
	}

	AMO_pairwise(U, E);
	AMO_pairwise(V, E);

	for (int i = 1; i <= p; i++) {
		for (int j = 1; j <= q; j++) {
			int k = (i - 1)*q + j;
			if (k <= t.size()) {
				vector<int> smc1;
				vector<int> smc2;
				smc1.push_back(-t[k - 1]);
				smc1.push_back(U[i - 1]);

				smc2.push_back(-t[k - 1]);
				smc2.push_back(V[j - 1]);

				E.push_back(smc1);
				E.push_back(smc2);
			}
		}
	}
}

void AMO_Seq(vector<int> t, int & newvar, vector<vector<int>>& E) {
	vector<int> aux_vars;
	for (int i = 0; i < t.size() - 1; i++) {
		aux_vars.push_back(newvar++);
	}
	vector<int> smc_first;
	smc_first.push_back(-t[0]);
	smc_first.push_back(aux_vars[0]);
	E.push_back(smc_first);

	vector<int> smc_last;
	smc_last.push_back(-t.back());
	smc_last.push_back(-aux_vars.back());
	E.push_back(smc_last);

	for (int i = 1; i < t.size() - 1; i++) {
		vector<int> smc1;
		smc1.push_back(-t[i]);
		smc1.push_back(aux_vars[i]);
		E.push_back(smc1);

		vector<int> smc2;
		smc2.push_back(-aux_vars[i - 1]);
		smc2.push_back(aux_vars[i]);
		E.push_back(smc2);

		vector<int> smc3;
		smc3.push_back(-t[i]);
		smc3.push_back(-aux_vars[i - 1]);
		E.push_back(smc3);

	}
}

int logm(int m) {
	int r = 0;
	int t = 1;
	while (t < m) {
		t = t * 2;
		r++;
	}
	return r;
}

void AMO_Bimander(vector<int> t, int m, int & newvar, vector<vector<int>> & E) {
	//m = number of sets
	vector<vector<int>> sets;
	for (int i = 0; i < m; i++) {
		vector<int> s_i;
		sets.push_back(s_i);
	}

	int k = 0;
	for (int i = 0; i < t.size(); i++) {
		sets[k].push_back(t[i]);
		k++;
		if (k == m) { k = 0; }
	}

	vector<int> bim_vars;
	int u = logm(m);
	for (int i = 0; i < u; i++) {
		bim_vars.push_back(newvar++);
	}

	for (int i = 0; i < m; i++) {
		AMO_pairwise(sets[i], E);
	}

	for (int i = 0; i < m; i++) {
		vector<vector<int>> BR = Bin_Rep(m);
		for (int h = 0; h < sets[i].size(); h++) {
			for (int j = 0; j < u; j++) {
				vector<int> smc;
				smc.push_back(-sets[i][h]);
				if (BR[i][j] == 1) {
					smc.push_back(bim_vars[j]);
				}
				else {
					smc.push_back(-bim_vars[j]);
				}
				E.push_back(smc);
			}
		}
	}
}



void AMO(vector<int> t, AMO_Type amo, int p, int q, int m, int & newvar, vector<vector<int>> & E) {
	switch (amo) {
	case AMO_Type::Pairwise: AMO_pairwise(t, E);  break;
	case AMO_Type::Binary: AMO_Binary(t, newvar, E); break;
	case AMO_Type::Product: AMO_Product(t, p, q, newvar, E); break;
	case AMO_Type::Commander: AMO_Commander(t, m, newvar, E); break;
	case AMO_Type::Bimander: AMO_Bimander(t, m, newvar, E); break;
	case AMO_Type::Sequential: AMO_Seq(t, newvar, E); break;
	}
}

void AMO(C_Cell t, AMO_Type amo, int p, int q, int m, int & newvar, vector<vector<int>> & E) {
	AMO(t.variables, amo, p, q, m, newvar, E);
}

void AMO(vector<C_Cell> t, AMO_Type amo, int p, int q, int m, int & newvar, vector<vector<int>> & E) {
	//here we suppose that it is either row or column
	int dim = t[0].variables.size();
	for (int i = 0; i < dim; i++) {
		vector<int> u;
		for (int j = 0; j < t.size(); j++) {
			u.push_back(t[j].variables[i]);
		}
		AMO(u, amo, p, q, m, newvar, E);
	}
}


void OnlyOne(vector<int> t, vector<vector<int>> & E) {
	vector<int> bigclause;
	for (int i = 0; i < t.size(); i++) {
		bigclause.push_back(t[i]);

		for (int j = i + 1; j < t.size(); j++) {
			vector<int> smallclause;
			smallclause.push_back(-t[i]);
			smallclause.push_back(-t[j]);
			E.push_back(smallclause);
		}
	}
	E.push_back(bigclause);
}

void OnlyOne(C_Cell t, vector<vector<int>> & E) {
	OnlyOne(t.variables, E);
}




void OnlyOne(vector<C_Cell> t, vector<vector<int>> & E) {
	//here we suppose that it is either row or column
	int dim = t[0].variables.size();
	for (int i = 0; i < dim; i++) {
		vector<int> u;
		for (int j = 0; j < t.size(); j++) {
			u.push_back(t[j].variables[i]);
		}
		OnlyOne(u, E);
	}
}



void var_eq_and(int leftpart, vector<int> rightpart, vector<vector<int>> & E) {
	vector<int> bigclause;
	bigclause.push_back(leftpart);
	for (int i = 0; i < rightpart.size(); i++) {
		bigclause.push_back(-rightpart[i]);
		vector<int> smallclause;
		smallclause.push_back(-leftpart);
		smallclause.push_back(rightpart[i]);
		E.push_back(smallclause);
	}
	E.push_back(bigclause);
}


void var_eq_or(int leftpart, vector<int> rightpart, vector<vector<int>> & E) {
	vector<int> bigclause;
	bigclause.push_back(-leftpart);
	for (int i = 0; i < rightpart.size(); i++) {
		bigclause.push_back(rightpart[i]);
		vector<int> smallclause;
		smallclause.push_back(leftpart);
		smallclause.push_back(-rightpart[i]);
		E.push_back(smallclause);
	}
	E.push_back(bigclause);
}

void var_eq_and(int lp, int rp1, int rp2, vector<vector<int>> & E) {
	vector<int> t;
	t.push_back(rp1);
	t.push_back(rp2);
	var_eq_and(lp, t, E);
}

void var_eq_or(int lp, int rp1, int rp2, vector<vector<int>> & E) {
	vector<int> t;
	t.push_back(rp1);
	t.push_back(rp2);
	var_eq_or(lp, t, E);
}

void var_eq_eq(int lp, int rp1, int rp2, vector<vector<int>> & E) {
	vector<int> smc1;
	vector<int> smc2;
	vector<int> smc3;
	vector<int> smc4;

	smc1.push_back(lp);
	smc1.push_back(rp1);
	smc1.push_back(rp2);
	
	smc2.push_back(lp);
	smc2.push_back(-rp1);
	smc2.push_back(-rp2);

	smc3.push_back(-lp);
	smc3.push_back(rp1);
	smc3.push_back(-rp2);

	smc4.push_back(-lp);
	smc4.push_back(-rp1);
	smc4.push_back(rp2);

	E.push_back(smc1);
	E.push_back(smc2);
	E.push_back(smc3);
	E.push_back(smc4);
}
int greater(B_Cell a, B_Cell b, int & nv_var, vector<vector<int>> & E) {
	vector<int> eqvars;
	for (int i = 0; i < a.variables.size(); i++) {
		eqvars.push_back(nv_var++);
	}
	for (int i = 0; i < a.variables.size(); i++) {
		var_eq_eq(eqvars[i], a.variables[i], b.variables[i], E);
	}

	vector<int> auxvars;
	for (int i = 0; i < a.variables.size(); i++) {
		auxvars.push_back(nv_var++);
	}

	for (int i = 0; i < a.variables.size(); i++) {
		vector<int> tmp;
		tmp.push_back(a.variables[i]);
		tmp.push_back(-eqvars[i]);
		for (int j = i - 1; j >= 0; j--) {
			tmp.push_back(eqvars[j]);
		}

		var_eq_and(auxvars[i], tmp, E);
	}
	int r = nv_var++;
	var_eq_or(r, auxvars, E);
	return r;
}

void var_eq_if(int lp, int cond, int rp1, int rp2, int &nv_var, vector<vector<int>>&E){
	int auxvar1 = nv_var++;
	int auxvar2 = nv_var++;

	var_eq_and(auxvar1, rp1, cond, E);
	var_eq_and(auxvar2, rp2, -cond, E);

	var_eq_or(lp, auxvar1, auxvar2, E);

}

void var_eq_if(B_Cell a, int cond, B_Cell rp1, B_Cell rp2, int &nv_var, vector<vector<int>>&E) {
	for (int i = 0; i < a.variables.size(); i++) {
		var_eq_if(a.variables[i], cond, rp1.variables[i], rp2.variables[i],nv_var, E);
	}
}
void encode_comparator(B_Cell in_a, B_Cell in_b, B_Cell out_a, B_Cell out_b, int &nv_var, vector<vector<int>>&E) {
	int bga = greater(in_b, in_a, nv_var, E);
	
	var_eq_if(out_a, bga, in_b, in_a, nv_var, E);
	var_eq_if(out_b, bga, in_a, in_b, nv_var, E);


}

