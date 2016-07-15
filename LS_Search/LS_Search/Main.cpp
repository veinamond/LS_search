#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <algorithm>
#include <sstream>
#include "encoding.h"
#include "minisat22_wrapper.h"
#include <random>

#include <bitset>
using namespace std;


//static inline double cpuTime(void) { return (double)clock() / CLOCKS_PER_SEC; }


vector<int> TR4{
	3,2,1,0,3,0,1,2,3,1,3,2,1,0,1,0,1,2,3,2,3,2,1,0,
	1,0,1,2,3,1,3,2,1,0,3,0,1,2,3,0,3,2,1,0,3,0,1,2,
	3,1,3,2,1,0,1,0,1,2,3,2,3,2,1,0,1,0,1,2,3,1,3,2,
	1,0,3,0,1,2,3,0,3,2,1,0,3,0,1,2,3,1,3,2,1,0,1,0,
	1,2,3,2,3,2,1,0,1,0,1,2,3,1,3,2,1,0,3,0,1,2,3,0 };
vector<int> TR2{
	0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4 };

void transform_2(int x, vector<vector<int>> & SQ) {
	for (int i = 0; i < 10; i++) swap(SQ[x][i], SQ[9 - x][i]);
	for (int i = 0; i < 10; i++) swap(SQ[i][x], SQ[i][9 - x]);
}

void transform_4(int x, vector<vector<int>> & SQ) {
	for (int i = 0; i < 10; i++) {
		swap(SQ[i][x], SQ[i][x + 1]);
		swap(SQ[i][9 - x], SQ[i][8 - x]);
	}
	for (int i = 0; i < 10; i++) {
		swap(SQ[x][i], SQ[x + 1][i]);
		swap(SQ[9 - x][i], SQ[8 - x][i]);
	}
}

vector<vector<int>> normalize_fr(vector<vector<int>> &SQ) {
	vector<vector<int>> temp(10, vector<int>(10));
	vector<int> color(10);
	for (int k = 0; k < 10; k++) color[SQ[0][k]] = k;
	//тут мы нормализуем по первой строке 
	for (int k = 0; k < 10; k++) {
		for (int l = 0; l < 10; l++) {
			temp[k][l] = color[SQ[k][l]];
		}
	}
	return temp;
}

int strtoi(string s) {
	int x = atoi(s.c_str());
	return x;
}
string inttostr(int number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
}

void EO(vector<int> vars, vector<vector<int>> & E) {
	vector<int> bigcl;
	for (int i = 0; i < vars.size(); i++) {
		bigcl.push_back(vars[i]);
	}
	E.push_back(bigcl);

	for (int i = 0; i < vars.size() - 1; i++) {
		for (int j = i + 1; j < vars.size(); j++) {
			vector<int> smcl;
			smcl.push_back(-vars[i]);
			smcl.push_back(-vars[j]);
			E.push_back(smcl);
		}
	}
}


vector<vector<int>> new_tv_encoding(int n) {
	vector<vector<int>> Encoding;
	vector<vector<vector<int>>> LS_vars(n, vector<vector<int>>(n, vector<int>(n)));
	int cnt = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < n; k++) {
				LS_vars[i][j][k] = ++cnt;
			}
		}
	}
	vector<vector<vector<int>>> TV_masks(n, vector<vector<int>>(n, vector<int>(n)));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < n; k++) {
				TV_masks[i][j][k] = ++cnt;
			}
		}
	}
	vector<vector<vector<int>>> TV_values(n, vector<vector<int>>(n, vector<int>(n)));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < n; k++) {
				TV_values[i][j][k] = ++cnt;
			}
		}
	}

	//encoding LS conditions
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			EO(LS_vars[i][j], Encoding);
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			vector<int> tmp;
			for (int k = 0; k < n; k++) {
				tmp.push_back(LS_vars[k][i][j]);
			}
			EO(tmp, Encoding);
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			vector<int> tmp;
			for (int k = 0; k < n; k++) {
				tmp.push_back(LS_vars[i][k][j]);
			}
			EO(tmp, Encoding);
		}
	}

	//encoding TV_masks conditions
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			EO(TV_masks[i][j], Encoding);
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			vector<int> tmp;
			for (int k = 0; k < n; k++) {
				tmp.push_back(TV_masks[k][i][j]);
			}
			EO(tmp, Encoding);
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			vector<int> tmp;
			for (int k = 0; k < n; k++) {
				tmp.push_back(TV_masks[i][k][j]);
			}
			EO(tmp, Encoding);
		}
	}

	//encoding constraints on TV_values
	//encoding TV_masks conditions
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			EO(TV_values[i][j], Encoding);
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			vector<int> tmp;
			for (int k = 0; k < n; k++) {
				tmp.push_back(TV_values[k][i][j]);
			}
			EO(tmp, Encoding);
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			vector<int> tmp;
			for (int k = 0; k < n; k++) {
				tmp.push_back(TV_values[i][k][j]);
			}
			EO(tmp, Encoding);
		}
	}


	//connection between everything
	for (int k = 0; k < n; k++) {//transversals
		for (int u = 0; u < n; u++) {
			for (int v = 0; v < n; v++) {
				//TV_values==v
				vector<int> tmp(n);
				for (int l = 0; l < n; l++) {
					tmp[l] = ++cnt;
					var_eq_and(tmp[l], LS_vars[u][l][v], TV_masks[k][u][l], Encoding);
				}
				var_eq_or(TV_values[k][u][v], tmp, Encoding);
			}
		}
	}

	ofstream out;
	string cnf_out = "D:\\LSTests\\tv_new_" + inttostr(n) + ".cnf";
	out.open(cnf_out);
	for (int i = 0; i < Encoding.size(); i++) {
		for (int j = 0; j < Encoding[i].size(); j++) {
			out << Encoding[i][j] << " ";
		}
		out << "0\n";
	}
	out.close();
	return Encoding;
}

vector<vector<int>> construct_cf(vector<vector<int>>&SQ, bool &acc) {
	// initialize
	vector<vector<vector<int>>> sources(4, vector<vector<int>>(10, vector<int>(10)));
	//vector<vector<int>> KF(10,vector<int>(10,10));
	vector<vector<int>> KF(SQ);

	for (int i = 0; i < 10; i++) for (int j = 0; j < 10; j++) {
		sources[0][i][j] = SQ[i][j];
		sources[1][j][i] = sources[0][i][j];

	}
	for (int i = 0; i < 10; i++) for (int j = 0; j < 10; j++) {
		sources[2][i][j] = sources[0][i][9 - j];
		sources[3][i][j] = sources[1][i][9 - j];
	}

	for (int u = 0; u < sources.size(); u++)
		for (int i = 0; i < TR4.size(); i++) {
			for (int j = 0; j < TR2.size(); j++) {
				vector<vector<int>> temp = normalize_fr(sources[u]);
				//нормализовали temp
				for (int k = 10; k < 100; k++) {
					if (KF[k / 10][k % 10] > temp[k / 10][k % 10]) {
						//лексикографически сравнили, если меньше, у нас новый канон
						for (int l = 0; l < 100; l++) KF[l / 10][l % 10] = temp[l / 10][l % 10];
						break;
					}
					else if (KF[k / 10][k % 10] < temp[k / 10][k % 10]) break;
				}
				transform_2(TR2[j], sources[u]);
				//что мы делаем тут ваще?
			}
			transform_4(TR4[i], sources[u]);
		}
	return KF;
}

vector<vector<int>> construct_cf_limited(vector<vector<int>>&SQ, bool &acc) {
	// initialize
	vector<vector<vector<int>>> sources(4, vector<vector<int>>(10, vector<int>(10)));
	//vector<vector<int>> KF(10,vector<int>(10,10));
	vector<vector<int>> KF(SQ);

	for (int i = 0; i < 10; i++) for (int j = 0; j < 10; j++) {
		sources[0][i][j] = SQ[i][j];
		sources[1][j][i] = sources[0][i][j];
	}
	for (int i = 0; i < 10; i++) for (int j = 0; j < 10; j++) {
		sources[2][i][j] = sources[0][i][9 - j];
		sources[3][i][j] = sources[1][i][9 - j];
	}

	for (int u = 0; u < sources.size(); u++)
		for (int i = 0; i < TR4.size(); i++) {
			for (int j = 0; j < TR2.size(); j++) {
				vector<vector<int>> temp = normalize_fr(sources[u]);
				//нормализовали temp
				for (int k = 10; k < 100; k++) {
					if (KF[k / 10][k % 10] > temp[k / 10][k % 10]) {
						acc = true;
						return KF;
						//лексикографически сравнили, если меньше, у нас новый канон
						for (int l = 0; l < 100; l++) KF[l / 10][l % 10] = temp[l / 10][l % 10];
						break;
					}
					else if (KF[k / 10][k % 10] < temp[k / 10][k % 10]) break;
				}
				transform_2(TR2[j], sources[u]);
				//что мы делаем тут ваще?
			}
			transform_4(TR4[i], sources[u]);
		}
	acc = false;
	return KF;
}



void generate_permutations(int n, vector<vector<int>> &perm, bool diag) {
	vector<int> seed;
	for (int i = 0; i < n; i++) {
		seed.push_back(i);
	}

	do {
		bool acc = true;
		if (diag == true) {
			int md = 0;
			int ad = 0;
			for (int j = 0; j < n; j++) {
				if (seed[j] == j) { md++; }
				if (seed[j] == n - j - 1) { ad++; }
			}
			if ((md != 1) || (ad != 1)) { acc = false; }
		}
		if (acc == true) { perm.push_back(seed); }
	} while (std::next_permutation(seed.begin(), seed.end()));
	cout << "Generated " << perm.size() << "permutations" << endl;
}
void construct_square_from_tv(vector<vector<int>> &tv_set, vector<int> &tv_ind, vector<vector<int>> &SQ) {
	sort(tv_ind.begin(), tv_ind.end());
	for (int i = 0; i < tv_ind.size(); i++) {
		for (int j = 0; j < tv_set[tv_ind[i]].size(); j++) {
			SQ[j][tv_set[tv_ind[i]][j]] = i;
		}
	}

	/*int n = tv_ind.size();
	cout << "Transversals " << endl;
	for (int i = 0; i < n; i++) {
	for (int j = 0; j < n; j++) {
	cout << tv_set[tv_ind[i]][j] << " ";
	}
	cout << endl;
	}

	cout << "Square" << endl;
	for (int i = 0; i < n; i++) {
	for (int j = 0; j < n; j++) {
	cout << SQ[i][j] << " ";
	}
	cout << endl;
	}
	*/
}


void construct_squares_from_tv_set(vector<vector<int>>&tv_set, vector<vector<int>> &tv_index_sets, vector<vector<vector<int>>> &SQUARES) {
	for (int i = 0; i < tv_index_sets.size(); i++) {
		construct_square_from_tv(tv_set, tv_index_sets[i], SQUARES[i]);
	}
}
void TV_check(vector<vector<int>> &TVSET, vector<vector<int>> &LS, vector<int> &indices) {
	int n = LS.size();

	for (int i = 0; i < TVSET.size(); i++) {
		vector<int> tmp(10);
		bool a = true;
		for (int u = 0; u < n; u++) {
			tmp[LS[u][TVSET[i][u]]]++;
			if (tmp[LS[u][TVSET[i][u]]]>1) {
				a = false;
				break;
			}
		}
		if (a == true) { indices.push_back(i); }
	}
}


class TRT {
public:
	TRT *parent;
	TRT *next_sibling;
	TRT *firstchild;
	int level;
	int value;
	int index;

};
void construct_TV(TRT *& r, vector<int> &v, vector<vector<int>> & TV) {
	v[r->level] = r->value;
	if (r->firstchild != NULL) {
		construct_TV(r->firstchild, v, TV);
		while (r->next_sibling != NULL) {
			r = r->next_sibling;
			construct_TV(r, v, TV);
		}
	}
	else {
		TV.push_back(v);
		cout << TV.size() << " " << r->index << endl;
	}

}
void addtv_to_TRT(TRT*&root, vector<int>& tv, int index, vector<TRT*> &elements) {
	TRT *cnode = root;

	for (int i = 0; i < tv.size(); i++) {
		if (cnode->firstchild == NULL) {
			TRT * nn = new (TRT);
			nn->level = i;
			nn->index = -1;
			nn->value = tv[i];
			nn->parent = cnode;
			nn->next_sibling = NULL;
			nn->firstchild = NULL;

			cnode->firstchild = nn;
			cnode = nn;
			elements.push_back(nn);
		}
		else {
			cnode = cnode->firstchild;
			bool b = false;
			while (b == false) {
				if (tv[i] == cnode->value) {
					b = true;
				}
				else {
					if (cnode->next_sibling != NULL) {
						cnode = cnode->next_sibling;
					}
					else {
						TRT * nn = new (TRT);
						nn->level = cnode->level;
						nn->index = -1;
						nn->value = tv[i];
						nn->parent = cnode->parent;
						cnode->next_sibling = nn;
						nn->next_sibling = NULL;
						nn->firstchild = NULL;
						cnode = nn;
						elements.push_back(nn);
					}
				}
			}
		}
	}
	cnode->index = index;
}
void construct_TV_from_TRT(int n, TRT *&root, vector<vector<int>> &TV) {
	vector<int> v(n);
	construct_TV(root, v, TV);
}

void construct_TR_tree(int n, TRT *& root, bool diag) {
	vector<vector<int>> pm;
	generate_permutations(n, pm, diag);
	sort(pm.begin(), pm.end());
	root = new(TRT);
	root->parent = NULL;
	root->firstchild = NULL;
	root->next_sibling = NULL;
	root->level = -1;
	root->value = -1;
	root->index = -1;

	vector<TRT*> elements;
	elements.push_back(root);
	for (int i = 0; i < pm.size(); i++) {
		addtv_to_TRT(root, pm[i], i, elements);
	}

	//	vector<vector<int>> r;
	//	construct_TV_from_TRT(n, root->firstchild, r);
}

void construct_TR_tree(vector<vector<int>>pm, TRT *& root, bool diag) {
	root = new(TRT);
	root->parent = NULL;
	root->firstchild = NULL;
	root->next_sibling = NULL;
	root->level = -1;
	root->value = -1;
	root->index = -1;

	vector<TRT*> elements;
	elements.push_back(root);
	for (int i = 0; i < pm.size(); i++) {
		addtv_to_TRT(root, pm[i], i, elements);
	}

	//	vector<vector<int>> r;
	//	construct_TV_from_TRT(n, root->firstchild, r);
}

void TV_check_TRT_mod(TRT *r, vector<vector<int>> &TVSet_SQUARE, vector<int>& ind, vector<int> &indices, vector<int> &tmp) {
	//tmp reflects if we already crossed some Masked_TV.
	int v = -1;

	for (int i = 0; i < ind.size(); i++) {
		if (TVSet_SQUARE[ind[i]][r->level] == r->value) {
			tmp[i]++;
			v = i;
			break;
		}
	}

	if (tmp[v] == 1) {
		//продолжаем только в этом случае		
		if (r->firstchild != NULL) {
			TV_check_TRT_mod(r->firstchild, TVSet_SQUARE, ind, indices, tmp);
		}
		else {
			indices.push_back(r->index);
		}
	}
	tmp[v]--;
	if (r->next_sibling != NULL) {
		r = r->next_sibling;
		TV_check_TRT_mod(r, TVSet_SQUARE, ind, indices, tmp);
	}
}


void TV_check_TRT(TRT *r, vector<vector<int>> &LS, vector<int> &indices, vector<int> &tmp) {
	//tmp holds LS elements that are already met on the partial Transversal
	tmp[LS[r->level][r->value]]++;
	if (tmp[LS[r->level][r->value]] == 1) {
		//продолжаем только в этом случае		
		if (r->firstchild != NULL) {
			TV_check_TRT(r->firstchild, LS, indices, tmp);
		}
		else {
			indices.push_back(r->index);
		}
	}
	tmp[LS[r->level][r->value]]--;
	if (r->next_sibling != NULL) {
		r = r->next_sibling;
		TV_check_TRT(r, LS, indices, tmp);
	}
}

void TV_check_TRT_masked(TRT *r, vector<vector<int>> &LSmask, vector<int> &indices, vector<int> &tmp) {
	//
	if (LSmask[r->level][r->value] == -1) {
		if (r->firstchild != NULL) {
			TV_check_TRT_masked(r->firstchild, LSmask, indices, tmp);
		}
		else {
			indices.push_back(r->index);
		}
	}
	else {
		tmp[LSmask[r->level][r->value]]++;
		if (tmp[LSmask[r->level][r->value]] == 1) {
			//продолжаем только в этом случае		
			if (r->firstchild != NULL) {
				TV_check_TRT_masked(r->firstchild, LSmask, indices, tmp);
			}
			else {
				indices.push_back(r->index);
			}
		}
		tmp[LSmask[r->level][r->value]]--;
	}
	if (r->next_sibling != NULL) {
		r = r->next_sibling;
		TV_check_TRT_masked(r, LSmask, indices, tmp);
	}
}




class DLX_column {
public:
	int size;
	int column_number;
	int row_id;

	DLX_column * Left;
	DLX_column * Right;
	DLX_column * Up;
	DLX_column * Down;
	DLX_column * Column;
};

void cover(DLX_column *&c) {
	//cout << "Covered " << c->column_number << endl;
	c->Right->Left = c->Left;
	c->Left->Right = c->Right;

	DLX_column *i;
	DLX_column *j;
	i = c->Down;
	while (i != c) {
		j = i->Right;
		while (j != i) {
			j->Down->Up = j->Up;
			j->Up->Down = j->Down;
			//	cout << "covered element " << j->row_id << " in column " << j->column_number << endl;
			j->Column->size--;
			if (j->Column->size < 0) {
				cout << "We are in deep trouble" << endl;
			}
			j = j->Right;
		}
		i = i->Down;
	}
}

void uncover(DLX_column *&c) {
	//cout << "Uncovered " << c->column_number << endl;
	DLX_column *i;
	DLX_column *j;
	i = c->Up;
	while (i != c) {
		j = i->Left;
		while (j != i) {
			j->Column->size++;

			j->Down->Up = j;
			j->Up->Down = j;

			j = j->Left;
		}
		i = i->Up;
	}
	c->Right->Left = c;
	c->Left->Right = c;
}



void choose_c(DLX_column &h, DLX_column *&c) {
	DLX_column * j;

	j = h.Right;
	int min = j->size;
	c = j;
	while (j != &h) {
		if (j->size < min) {
			c = j;
			min = j->size;
		}
		j = j->Right;
	}
}

void print_solution(vector<DLX_column*> &ps) {
	cout << endl;
	for (int i = 0; i < ps.size(); i++) {
		cout << ps[i]->row_id << " ";
	}
	cout << endl;
}

void search_limited(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr, bool &cont, unsigned long long &limit, bool &count_only, unsigned long long &count) {
	//pd = partial solution
	if (k > 10) {
		cout << "we are in trouble" << endl;

	}
	//	cout << "Search " << k << endl;
	if (cont == true) {
		if (h.Right == &h) {
			count++;
			if (count_only == false) {
				vector<int> tmpv;
				for (int i = 0; i < ps.size(); i++) {
					tmpv.push_back(ps[i]->row_id);
				}
				tvr.push_back(tmpv);
			}
			if (count > limit) { cont = false; }
			if (count % 10000000 == 0) { cout << count << endl; }
			//print_solution(ps);
		}
		else {
			DLX_column * c = NULL;
			choose_c(h, c);
			//cout << "picked column " << c->column_number << endl;
			cover(c);
			DLX_column * r = c->Down;
			while ((r != c) && (cont == true)) {
				ps.push_back(r);
				DLX_column * j;
				j = r->Right;
				while (j != r) {
					cover(j->Column);
					j = j->Right;
				}

				search_limited(k + 1, h, ps, tvr, cont, limit, count_only, count);
				r = ps.back();
				//questionable.
				ps.pop_back();
				c = r->Column;

				j = r->Left;
				while (j != r) {
					uncover(j->Column);
					j = j->Left;
				}
				r = r->Down;
			}
			uncover(c);
			//return;
		}
	}
}



void search(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr) {
	//pd = partial solution
	if (k > 10) {
		cout << "we are in trouble" << endl;

	}
	//	cout << "Search " << k << endl;
	if (h.Right == &h) {
		vector<int> tmpv;
		for (int i = 0; i < ps.size(); i++) {
			tmpv.push_back(ps[i]->row_id);
		}
		tvr.push_back(tmpv);
		//cout << tvr.size() << endl;
		//print_solution(ps);
	}
	else {
		DLX_column * c = NULL;
		choose_c(h, c);
		//	cout << "picked column " << c->column_number << endl;
		cover(c);
		DLX_column * r = c->Down;
		while (r != c) {
			ps.push_back(r);
			DLX_column * j;
			j = r->Right;
			while (j != r) {
				cover(j->Column);
				j = j->Right;
			}

			search(k + 1, h, ps, tvr);

			r = ps.back();
			//questionable.
			ps.pop_back();
			c = r->Column;

			j = r->Left;
			while (j != r) {
				uncover(j->Column);
				j = j->Left;
			}
			r = r->Down;
		}
		uncover(c);
		//return;
	}
}

void TVSET_TO_DLX(DLX_column &root, vector<vector<int>> & tvset, vector<DLX_column*> & elements) {
	int dimension = tvset[0].size();
	root.Up = NULL;
	root.Down = NULL;
	root.Column = NULL;
	root.row_id = -1;
	root.size = -1;
	//	root.column_number= -1;
	elements.push_back(&root);
	vector<DLX_column *> columns;
	DLX_column * lastleft = &root;
	for (int i = 0; i < dimension* dimension; i++) {
		DLX_column *ct;
		ct = new (DLX_column);
		//	ct->column_number = i;
		ct->Down = ct;
		ct->Up = ct;
		ct->size = 0;
		ct->row_id = 0;
		ct->Column = ct;
		ct->Left = lastleft;
		lastleft->Right = ct;
		lastleft = ct;
		columns.push_back(ct);
		elements.push_back(ct);
	}
	lastleft->Right = &root;
	root.Left = lastleft;

	for (int i = 0; i < tvset.size(); i++) {
		vector<int> curtv = tvset[i];
		vector<DLX_column *> tvrow;
		for (int j = 0; j < curtv.size(); j++) {
			DLX_column *ctve;
			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind
			int k = j*dimension + curtv[j];

			ctve->Column = columns[k];
			ctve->Column->size++;
			ctve->Down = columns[k];
			ctve->Up = columns[k]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);
		}

		for (int j = 0; j < tvrow.size() - 1; j++) {
			tvrow[j]->Right = tvrow[j + 1];
			tvrow[j]->Right->Left = tvrow[j];
		}
		tvrow[tvrow.size() - 1]->Right = tvrow[0];
		tvrow[0]->Left = tvrow[tvrow.size() - 1];
	}
	DLX_column *pr = &root;

}

void SQ_TO_DLX(DLX_column &root, vector<vector<int>> & SQ, vector<DLX_column*> & elements) {
	int dimension = SQ[0].size();
	root.Up = NULL;
	root.Down = NULL;
	root.Column = NULL;
	root.row_id = -1;
	root.size = -1;
	//	root.column_number= -1;
	elements.push_back(&root);
	vector<DLX_column *> columns;
	DLX_column * lastleft = &root;
	// first n - row number
	// n to 2n - column number
	//2n to 3n - value
	//3n+1 - diag
	//3n+2 - antidiag
	for (int i = 0; i < 3*dimension+2; i++) {
		DLX_column *ct;
		ct = new (DLX_column);
		//	ct->column_number = i;
		ct->Down = ct;
		ct->Up = ct;
		ct->size = 0;
		ct->row_id = 0;
		ct->Column = ct;
		ct->Left = lastleft;
		lastleft->Right = ct;
		lastleft = ct;
		columns.push_back(ct);
		elements.push_back(ct);
	}
	lastleft->Right = &root;
	root.Left = lastleft;
	for (int i = 0; i < SQ.size(); i++) {
		for (int j = 0; j < SQ[i].size(); j++) {
			vector<DLX_column *> tvrow;
			DLX_column *ctve;
			ctve = new (DLX_column);			
						
			ctve->Column = columns[i];
			ctve->Column->size++;
			ctve->Down = columns[i];
			ctve->Up = columns[i]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i*dimension +j;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);

			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind

			ctve->Column = columns[dimension + j];
			ctve->Column->size++;
			ctve->Down = columns[dimension + j];
			ctve->Up = columns[dimension + j]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i*dimension + j;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);

			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind

			ctve->Column = columns[2*dimension + SQ[i][j]];
			ctve->Column->size++;
			ctve->Down = columns[2 * dimension + SQ[i][j]];
			ctve->Up = columns[2 * dimension + SQ[i][j]]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i*dimension + j;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);

			if (i == j) {
				ctve = new (DLX_column);
				ctve->Column = columns[3 * dimension ];
				ctve->Column->size++;
				ctve->Down = columns[3 * dimension ];
				ctve->Up = columns[3 * dimension]->Up;
				ctve->Up->Down = ctve;
				ctve->Down->Up = ctve;
				ctve->row_id = i*dimension + j;
				//	ctve->column_number = k;
				ctve->size = -10;
				elements.push_back(ctve);
				tvrow.push_back(ctve);
			}

			if (i == (dimension - j - 1)) {
				ctve = new (DLX_column);
				ctve->Column = columns[3 * dimension+1];
				ctve->Column->size++;
				ctve->Down = columns[3 * dimension+1];
				ctve->Up = columns[3 * dimension+1]->Up;
				ctve->Up->Down = ctve;
				ctve->Down->Up = ctve;
				ctve->row_id = i*dimension + j;
				//	ctve->column_number = k;
				ctve->size = -10;
				elements.push_back(ctve);
				tvrow.push_back(ctve);
			}			

			for (int j = 0; j < tvrow.size() - 1; j++) {
				tvrow[j]->Right = tvrow[j + 1];
				tvrow[j]->Right->Left = tvrow[j];
			}
			tvrow[tvrow.size() - 1]->Right = tvrow[0];
			tvrow[0]->Left = tvrow[tvrow.size() - 1];

		}
	}
	DLX_column *pr = &root;

}


void TVSET_TO_DLX_EXT(DLX_column &root, vector<vector<int>> & tvset, vector<DLX_column*> & columns, vector<vector<DLX_column*>> &rows) {
	int dimension = tvset[0].size();
	root.Up = NULL;
	root.Down = NULL;
	root.Column = NULL;
	root.row_id = -1;
	root.size = -1;
	//	root.column_number= -1;
	columns.clear();
	rows.clear();
	DLX_column * lastleft = &root;
	for (int i = 0; i < dimension* dimension; i++) {
		DLX_column *ct;
		ct = new (DLX_column);
		//	ct->column_number = i;
		ct->Down = ct;
		ct->Up = ct;
		ct->size = 0;
		ct->row_id = 0;
		ct->Column = ct;
		ct->Left = lastleft;
		lastleft->Right = ct;
		lastleft = ct;
		columns.push_back(ct);
	}
	lastleft->Right = &root;
	root.Left = lastleft;

	for (int i = 0; i < tvset.size(); i++) {
		vector<int> curtv = tvset[i];
		vector<DLX_column *> tvrow;
		for (int j = 0; j < curtv.size(); j++) {
			DLX_column *ctve;
			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind
			int k = j*dimension + curtv[j];

			ctve->Column = columns[k];
			ctve->Column->size++;
			ctve->Down = columns[k];
			ctve->Up = columns[k]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i;
			//	ctve->column_number = k;
			ctve->size = -10;
			tvrow.push_back(ctve);
		}

		for (int j = 0; j < tvrow.size() - 1; j++) {
			tvrow[j]->Right = tvrow[j + 1];
			tvrow[j]->Right->Left = tvrow[j];
		}
		tvrow[tvrow.size() - 1]->Right = tvrow[0];
		tvrow[0]->Left = tvrow[tvrow.size() - 1];
		rows.push_back(tvrow);
	}
}



static bool nextij(int &i, int &j, vector<int> &ct, vector<vector<int>> & a, vector<bool>&co, vector<bool> & x) {
	int dim = x.size();
	bool r = false;

	while ((r == false) && (j < dim)) {
		j++;
		if (j == 10) {
			i--;
			if (i < 0) {
				r = true;
			}
			else {
				co[ct[i]] = false;
				x[a[i][ct[i]]] = false;
				j = ct[i];
			}
		}
		else {
			if ((co[j] == false) && (x[a[i][j]] == false)) {
				co[j] = true;
				x[a[i][j]] = true;
				r = true;
			}
		}
	}
	if (i < 0)
	{
		return false;
	}
	else return true;
}

bool initTrans(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x) {
	int i = 0;
	int j = -1;
	for (int i = 0; i < x.size(); i++) {
		co[i] = false;
		x[i] = false;
		ct[i] = 0;
	}
	while ((i<10) && (nextij(i, j, ct, a, co, x) != false)) {
		ct[i] = j;
		i++;
		j = -1;
	}
	if (i == 10) return true;
	else return false;
}

bool nextTrans(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x) {
	// a - square
	// ct = current transversal, coordinates correspond to column number
	// co - column occupied vector
	// x - aux vector
	int d = ct.size() - 1;
	int i = d - 1;
	x[a[d][ct[d]]] = false;
	x[a[i][ct[i]]] = false;
	co[ct[i]] = false;
	co[ct[d]] = false;
	int j = ct[i];
	bool out = true;
	bool res = false;

	while (!res) {
		if (nextij(i, j, ct, a, co, x) == true) {


			ct[i] = j;
			i++;
			j = -1;
			if (i == 10) res = true;
		}
		else {
			res = true;
			out = false;
		}

	}
	return out;
}

bool nextTrans_ext(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x, bool &diag) {
	// a - square
	// ct = current transversal, coordinates correspond to column number
	// co - column occupied vector
	// x - aux vector
	int d = ct.size() - 1;
	int i = d - 1;
	x[a[d][ct[d]]] = false;
	x[a[i][ct[i]]] = false;
	co[ct[i]] = false;
	co[ct[d]] = false;
	int j = ct[i];
	bool out = true;
	bool res = false;

	while (!res) {
		if (nextij(i, j, ct, a, co, x) == true) {

			ct[i] = j;
			i++;
			j = -1;
			if ((i == 10) && (diag == false)) { res = true; }
			if ((i == 10) && (diag == true)) {
				int md = 0;
				int ad = 0;
				for (int l = 0; l < ct.size(); l++) {
					if (ct[l] == l) { md++; }
					if (ct[l] == ct.size() - l - 1) { ad++; }
				}
				if ((md == 1) && (ad == 1)) { res = true; }
			}
		}
		else {
			res = true;
			out = false;
		}

	}
	return out;
}

void printvector(vector<int> t) {
	for (int i = 0; i < t.size(); i++) {
		cout << t[i] << " ";
	}
}

vector<vector<int>> getTrans_mod(vector<vector<int>> a, bool diag) {
	vector<vector<int>> res(5600, vector<int>(10));
	vector<int> ltr(10);
	vector<int> h(10);
	vector<bool> co(10);
	vector<bool> x(10);

	int num = 0;

	//bool t = initTrans(a, h, co, x);
	bool t = initTrans(a, h, co, x);
	if (t == true) {
		bool acc = true;
		if (diag == true) {
			int md = 0;
			int ad = 0;
			for (int l = 0; l < h.size(); l++) {
				if (h[l] == l) { md++; }
				if (h[l] == h.size() - l - 1) { ad++; }
			}
			if ((md != 1) || (ad != 1)) { acc = false; }
		}
		if (acc == true) {
			res[num] = h;
			num++;
		}

		//	while (nextTrans(a, h, co, x) == true) {
		while (nextTrans_ext(a, h, co, x, diag) == true) {
			res[num] = h;
			num++;


		}
	}
	res.erase(res.begin() + num, res.end());
	return res;
}

int indexof(vector<vector<int>> & TVSET, vector<int> & tv) {
	bool found = false;
	int res = -1;
	int dim = TVSET.size() / 2;
	int k = dim;
	//cout <<"Searching for:"<< endl;
	//printvector(tv);
	//cout <<endl;
	while ((k >= 0) && (found == false)) {
		if (dim < 3) {
			for (int j = k - 4; j <= k + 4; j++) {
				//	printvector(TVSET[j]);
				//	cout << endl;
				if (TVSET[j] == tv) {
					found = true;
					res = j;
					return j;
				}
			}
			if (found == false) {
				cout << "FAIL" << endl;
			}
		}
		else {
			int r = dim % 2;
			dim = dim / 2;
			if (tv > TVSET[k]) {
				k = k + dim - r;
			}
			else {
				k = k - dim + r;
			}
		}
	}
	return res;
}

vector<int> getTrans_mod_ind(vector<vector<int>> &a, vector<vector<int>> &TVSET, bool diag) {
	vector<int> h(10);
	vector<bool> co(10);
	vector<bool> x(10);
	vector<int> res(5600);
	int num = 0;
	//bool t = initTrans(a, h, co, x);
	bool t = initTrans(a, h, co, x);
	if (t == true) {
		bool acc = true;
		if (diag == true) {
			int md = 0;
			int ad = 0;
			for (int l = 0; l < h.size(); l++) {
				if (h[l] == l) { md++; }
				if (h[l] == h.size() - l - 1) { ad++; }
			}
			if ((md != 1) || (ad != 1)) { acc = false; }
		}
		if (acc == true) {
			res[num] = indexof(TVSET, h);
			num++;
		}

		//	while (nextTrans(a, h, co, x) == true) {
		while (nextTrans_ext(a, h, co, x, diag) == true) {
			res[num] = indexof(TVSET, h);
			num++;
		}
	}
	res.resize(num);
	return res;
}

vector<int> getTrans_mod_ind_tv(vector<int> &tvind, vector<vector<int>> &TVSET, vector<vector<int>> &TVSET_SQUARE, bool diag) {
	vector<int> h(10);
	vector<bool> co(10);
	vector<bool> x(10);
	vector<int> res(5600);
	int num = 0;
	vector<vector<int>> a(10, vector<int>(10));
	construct_square_from_tv(TVSET_SQUARE, tvind, a);

	//bool t = initTrans(a, h, co, x);
	bool t = initTrans(a, h, co, x);
	if (t == true) {
		bool acc = true;
		if (diag == true) {
			int md = 0;
			int ad = 0;
			for (int l = 0; l < h.size(); l++) {
				if (h[l] == l) { md++; }
				if (h[l] == h.size() - l - 1) { ad++; }
			}
			if ((md != 1) || (ad != 1)) { acc = false; }
		}
		if (acc == true) {
			res[num] = indexof(TVSET, h);
			num++;
		}

		//	while (nextTrans(a, h, co, x) == true) {
		while (nextTrans_ext(a, h, co, x, diag) == true) {
			res[num] = indexof(TVSET, h);
			num++;
		}
	}
	res.resize(num);
	return res;
}





void getTrans(vector<vector<int>> a, bool diag, vector<vector<int>> &tr, vector<int> &ltr, vector<int> &hb) {
	int i;
	int j;
	int k;
	int c;
	int d;
	int t;
	vector<int> h(10);
	vector<bool> x(10);

	hb[0] = 0;
	for (t = 0; t < 10; t++) {
		for (k = 0; k < 10; k++) {
			x[k] = false;
		}
		c = 0;
		h[0] = t;
		x[a[0][t]] = true;

		i = 1;
		h[i] = 0;

	label1:
		if (i == 0) goto label4;
		j = h[i];
	label2:
		if (j == 10) {
			i--;
			x[a[i][h[i]]] = false;
			h[i]++;
			goto label1;
		}
		if (x[a[i][j]] == true) {
			j++;
			goto label2;
		}
		for (k = 0; k < i; k++) {
			if (h[k] == j) {
				j++;
				goto label2;
			}
		}

		h[i] = j;
		x[a[i][j]] = true;

		if (i == 9) {
			if (diag == true) {
				d = 0;
				for (k = 0; k < 10; k++) {
					if (k == h[k]) {
						d++;
					}
				}
				if (d != 1)
					goto label3;

				d = 0;
				for (k = 0; k < 10; k++) {
					if (k == h[9 - k]) {
						d++;
					}
				}
				if (d != 1)
					goto label3;
			}
			//cout << "Transversal found!" << endl;
			for (k = 0; k < 10; k++) {
				//		cout << h[k] << " ";
				tr[hb[t] + c][k] = h[k];
			}
			//	cout << endl;
			c++;
		}

	label3:
		if (i == 9)
		{
			x[a[i][h[i]]] = false;
			j++;
			goto label2;
		}
		i++;
		h[i] = 0;
		goto label1;

	label4:
		ltr[t] = c;
		//cout << c << endl;
		hb[t + 1] = hb[t] + c;
	}
}
/*vector<vector<int>> tr(10000, vector<int>(10));
vector<int> ltr(10);
vector<int> hb(11);
vector<vector<vector<int>>> orts(10000);*/
int getOrt(vector<vector<int>> &trv, vector<int> &ltr, vector<vector<int>>&tr, vector<vector<vector<int>>> &orts) {
	int i;
	int ii;
	int k;
	int l;
	vector<int> h(11);
	vector<int> hb(11);
	vector<vector<int>> b(10, vector<int>(10));
	int c;

	hb[0] = 0;
	c = 0;
	for (k = 1; k <= 10; k++) {
		hb[k] = hb[k - 1] + ltr[k - 1];
	}
	i = 0;
	h[0] = 0;

label1:
	//{i - ый блок от hb[i] до hb[i + 1] - 1, h[i] - рассматриваемая тр.}
	//h - это массив индексов трансверсалей, которые образуют текущее разбиение
	ii = h[i];
label2:

	if (ii >= hb[i + 1]) {
		if (i == 0) {
			return c;
		}
		//cout << endl << "exceeded block " << i;
		i--;
		h[i]++;
		goto label1;
	}
	//{сравнение с предыдущими трансверсалями}
	for (k = 0; k < i; k++) {
		for (l = 0; l < 10; l++) {
			if (tr[h[k]][l] == tr[ii][l]) {
				ii++;
				goto label2;
			}
		}
	}
	h[i] = ii;
	if (i == 9) {
		trv.push_back(h);
		for (k = 0; k < 10; k++) {
			//	cout << h[k] << " ";
			for (l = 0; l < 10; l++) {
				b[l][tr[h[k]][l]] = k;
			}
		}
		//	cout << endl;
		c++;
		vector<vector<int>> a = b;
		orts[c] = a;
		//cout << c << endl;
		h[i]++;
		goto label1;
	}
	i++;
	h[i] = hb[i];
	goto label1;
}

void read_pairs_from_file(string filename, vector<vector<vector<int>>> &Squares) {
	ifstream in;
	in.open(filename);
	vector<int> a;
	string s;
	while (in.good()) {
		getline(in, s);
		if (s.length()>2) {
			int k = 0;
			bool b = true;
			while (b == true) {
				if (s[k] != ' ') {
					string t = s.substr(k, 1);
					int r = strtoi(t);
					a.push_back(r);
				}
				k++;
				if (k == s.length()) { b = false; }
			}
		}
	}
	in.close();

	for (int t = 0; t < (a.size()) / 200; t++) {
		vector<vector<int>> LS1;
		vector<vector<int>> LS2;
		for (int i = 0; i < 10; i++) {
			vector<int> tmp1;
			vector<int> tmp2;
			for (int j = 0; j < 10; j++) {
				tmp1.push_back(a[t * 200 + i * 20 + j]);

				tmp2.push_back(a[t * 200 + i * 20 + 10 + j]);
			}
			LS1.push_back(tmp1);
			LS2.push_back(tmp2);
		}
		Squares.push_back(LS1);
		Squares.push_back(LS2);
		/*cout << "LS1" << endl;
		for (int u = 0; u < 10; u++) {
		for (int v = 0; v < 10; v++) {
		cout << LS1[u][v] << " ";
		}
		cout << endl;
		}

		cout << "LS2" << endl;
		for (int u = 0; u < 10; u++) {
		for (int v = 0; v < 10; v++) {
		cout << LS2[u][v] << " ";
		}
		cout << endl;
		}
		*/
	}
	/*
	for (int i = 0; i < Squares.size(); i++) {
	string fn = "D:\\LSTests\\LSD10_square" + inttostr(i);
	ofstream out;
	out.open(fn);
	for (int u = 0; u < 10; u++) {
	for (int v = 0; v < 10; v++) {
	out << Squares[i][u][v] << " ";
	}
	out << "\n";
	}
	out.close();
	}
	*/
}

void read_squares_from_file(string filename, vector<vector<vector<int>>> &Squares) {
	ifstream in;
	in.open(filename);
	vector<int> a;
	string s;
	while (in.good()) {
		getline(in, s);
		if (s.length()>2) {
			int k = 0;
			bool b = true;
			while (b == true) {
				if (s[k] != ' ') {
					string t = s.substr(k, 1);
					int r = strtoi(t);
					a.push_back(r);
				}
				k++;
				if (k == s.length()) { b = false; }
			}
		}
	}
	in.close();

	for (int t = 0; t < (a.size()) / 100; t++) {
		vector<vector<int>> LS1;
		for (int i = 0; i < 10; i++) {
			vector<int> tmp1;
			for (int j = 0; j < 10; j++) {
				tmp1.push_back(a[t * 100 + i * 10 + j]);
			}
			LS1.push_back(tmp1);

		}
		Squares.push_back(LS1);
		/*cout << "LS1" << endl;
		for (int u = 0; u < 10; u++) {
		for (int v = 0; v < 10; v++) {
		cout << LS1[u][v] << " ";
		}
		cout << endl;
		}
		*/
	}
	/*
	for (int i = 0; i < Squares.size(); i++) {
	string fn = "D:\\LSTests\\LSD10_square" + inttostr(i);
	ofstream out;
	out.open(fn);
	for (int u = 0; u < 10; u++) {
	for (int v = 0; v < 10; v++) {
	out << Squares[i][u][v] << " ";
	}
	out << "\n";
	}
	out.close();
	}
	*/
}


void check_belyaev(vector<vector<int>> SQ, bool diag, string filename) {
	ofstream out;

	vector<vector<int>> tr(10000, vector<int>(10));
	vector<int> ltr(10);
	vector<int> hb(11);
	vector<vector<vector<int>>> orts(10000);
	getTrans(SQ, diag, tr, ltr, hb);
	int sum = 0;
	for (int i = 0; i < ltr.size(); i++) {
		sum += ltr[i];
	}
	
	vector<vector<int>> trv;
	int t = getOrt(trv, ltr, tr, orts);
	if (trv.size() != 0) {
		cout << "LS with orthogonal mates found \n";
		out.open(filename, ios::app);
		for (int i = 0; i < SQ.size(); i++) {
			for (int j = 0; j < SQ[i].size(); j++) {
				out << SQ[i][j] << " ";
			}
			out << endl;
		}
		out << "(Belyaev)Total: " << sum << " transversals" << endl;
		out << "Found " << trv.size() << "sets of disjoint transversals" << endl;
		if (trv.size()>0) {
			for (int i = 0; i < trv.size(); i++) {
				for (int j = 0; j < trv[i].size(); j++) {
					out << trv[i][j] << " ";
				}
				out << endl;
			}
		}
		out.close();
	}

}

vector<vector<int>> find_tv_dlx(int n, vector<vector<int>> &SQ) {
	
	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	SQ_TO_DLX(*root, SQ, elements);
	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	search(0, *root, ps, tvr);

	
	//cout << "Found " << tvr.size() << " transversals\n";
	for (int i = 0; i < tvr.size(); i++) {		
		sort(tvr[i].begin(), tvr[i].end());
		for (int j = 0; j < tvr[i].size(); j++) {
			tvr[i][j] = tvr[i][j] % n;
		}
		//printvector(tvr[i]);
		//cout << endl;
	}
	for (auto i = 0; i < elements.size(); i++) {
		delete elements[i];
	}
	elements.clear();
	return tvr;
}


void check_dlx(vector<vector<int>> SQ, bool diag, string filename) {
	ofstream out;


	vector<vector<int>> trm = getTrans_mod(SQ, diag);

	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, trm, elements);
	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	search(0, *root, ps, tvr);
	for (int i = 0; i < tvr.size(); i++) {
		sort(tvr[i].begin(), tvr[i].end());
	}

	for (int i = 0; i < elements.size(); i++) {
		delete elements[i];
	}
	elements.clear();

	if (tvr.size()>0) {
		cout << "LS with orthogonal mates found \n";
		out.open(filename, ios::app);
		for (int i = 0; i < SQ.size(); i++) {
			for (int j = 0; j < SQ[i].size(); j++) {
				out << SQ[i][j] << " ";
			}
			out << endl;
		}
		out << "Found " << tvr.size() << "sets of disjoint transversals" << endl;
		out << "(DLX_OLD)Total: " << trm.size() << " transversals" << endl;

		for (int i = 0; i < tvr.size(); i++) {
			for (int j = 0; j < tvr[i].size(); j++) {
				out << tvr[i][j] << " ";
			}
			out << endl;
		}
	}
	out.close();
}

void check_dlx_rc1(vector<vector<int>> SQ, bool diag, string filename) {
	ofstream out;


	vector<vector<int>> trm = find_tv_dlx(SQ.size(), SQ);
	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, trm, elements);
	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	search(0, *root, ps, tvr);
	for (int i = 0; i < tvr.size(); i++) {
		sort(tvr[i].begin(), tvr[i].end());
	}

	for (int i = 0; i < elements.size(); i++) {
		delete elements[i];
	}

	if (tvr.size()>0) {
		cout << "LS with orthogonal mates found \n";
		out.open(filename, ios::app);
		for (int i = 0; i < SQ.size(); i++) {
			for (int j = 0; j < SQ[i].size(); j++) {
				out << SQ[i][j] << " ";
			}
			out << endl;
		}
		out << "Found " << tvr.size() << "sets of disjoint transversals" << endl;
		out << "(DLX_OLD)Total: " << trm.size() << " transversals" << endl;

		for (int i = 0; i < tvr.size(); i++) {
			for (int j = 0; j < tvr[i].size(); j++) {
				out << tvr[i][j] << " ";
			}
			out << endl;
		}
	}
	out.close();
}


void clear_links_DLX(DLX_column *&root, vector<DLX_column*> &columns) {
	for (int i = 0; i < columns.size(); i++) {
		DLX_column * ct = columns[i];
		ct->Down = ct;
		ct->Up = ct;
		ct->size = 0;
	}
}

void configure_DLX(DLX_column *&root, vector<DLX_column*>& columns, vector<vector<DLX_column*>> &rows, vector<int> &tvind) {
	clear_links_DLX(root, columns);
	for (int i = 0; i < tvind.size(); i++) {
		vector<DLX_column*> cur_row = rows[tvind[i]];
		for (int j = 0; j < cur_row.size(); j++) {
			DLX_column* ctve = cur_row[j];
			ctve->Column->size++;
			ctve->Down = ctve->Column;
			ctve->Up = ctve->Column->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
		}
	}
}

void restore_DLX(DLX_column *&root, vector<DLX_column*> &columns, vector<vector<DLX_column*>> &rows) {
	for (int i = 0; i < rows.size(); i++) {
		vector<DLX_column*> cur_row = rows[i];
		for (int j = 0; j < cur_row.size(); j++) {
			DLX_column* ctve = cur_row[j];
			ctve->Column->size++;
			ctve->Down = ctve->Column;
			ctve->Up = ctve->Column->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
		}
	}
}




void check_dlx_EXT(int index, TRT *&r, vector<int> &TVind, vector<vector<int>>& TVSET, vector<vector<int>>& TVset_SQUARE, DLX_column *&root, vector<DLX_column*>& columns, vector<vector<DLX_column*>>&rows, bool diag, string filename) {


	//vector<int> trm;
	//vector<int> tm(10);
	//TV_check_TRT_mod(r->firstchild, TVset_SQUARE, TVind, trm, tm);


	vector<vector<int>> SQ(10, vector<int>(10));
	construct_square_from_tv(TVset_SQUARE, TVind, SQ);
	//bool t = false;
	//SQ = construct_cf_limited(SQ, t);	
	//BACKTRACK

	vector<int> trm;
	vector<int> tmp(10);
	TV_check_TRT(r->firstchild, SQ, trm, tmp);
	//= getTrans_mod_ind_tv(TVind, TVSET, TVset_SQUARE, diag);
	//configure_DLX(root, columns, rows, trm);

	configure_DLX(root, columns, rows, trm);

	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;

	search(0, *root, ps, tvr);
	for (int i = 0; i < tvr.size(); i++) {
		sort(tvr[i].begin(), tvr[i].end());
	}

	if (tvr.size()>0) {
		cout << "LS with orthogonal mates found \n";
		ofstream out;
		out.open(filename, ios::app);
		out << "Square " << index << endl;
		//vector<vector<int>> SQ(10, vector<int>(10));
		//construct_square_from_tv(TVset_SQUARE, TVind, SQ);
		for (int i = 0; i < SQ.size(); i++) {
			for (int j = 0; j < SQ.size(); j++) {
				out << SQ[i][j] << " ";
			}
			out << endl;
		}
		out << "(DLX_new)Total: " << trm.size() << " transversals" << endl;
		out << "Found " << tvr.size() << "sets of disjoint transversals" << endl;
		for (int i = 0; i < tvr.size(); i++) {
			for (int j = 0; j < tvr[i].size(); j++) {
				out << tvr[i][j] << " ";
			}
			out << endl;
		}
		out.close();
	}


}

void check_squares_Belyaev(vector<vector<vector<int>>> Squares, bool diag, string logfilename) {
	ofstream out;
	double t_start = cpuTime();
	for (int i = 0; i < Squares.size(); i++) {
		check_belyaev(Squares[i], diag, logfilename);
	}
	double t_end = cpuTime();
	out.open(logfilename, ios::app);
	out << "(Belyaev)Total time required to process the set is " << t_end - t_start << " seconds" << endl;
	out.close();
}

void check_squares_DLX(vector<vector<vector<int>>> Squares, bool diag, string logfilename) {
	ofstream out;

	double t_start = cpuTime();

	for (int i = 0; i < Squares.size(); i++) {
		if ((i > 0) && (i % 1000 == 0)) { cout << "Checked " << i << " squares" << endl; }
		check_dlx(Squares[i], diag, logfilename);
	}
	double t_end = cpuTime();
	out.open(logfilename, ios::app);
	out << "(DLX_old)Total time required to process the set is " << t_end - t_start << " seconds" << endl;
	out.close();
}


void check_squares_DLX_EXT_tv(TRT*& r, vector<vector<int>> & tvinds, vector<vector<int>>& TVSET, vector<vector<int>>& TVSET_SQUARE, DLX_column *&root, vector<DLX_column*>& columns, vector<vector<DLX_column*>>&rows, bool diag, string logfilename) {
	ofstream out;
	double t_start = cpuTime();

	for (int i = 0; i < tvinds.size(); i++) {
		if ((i > 0) && (i % 1000 == 0)) { cout << "Checked " << i << " squares" << endl; }
		//	out.open(logfilename, ios::app);
		//	out << endl << "Square " << i << endl;
		//	out.close();
		check_dlx_EXT(i, r, tvinds[i], TVSET, TVSET_SQUARE, root, columns, rows, diag, logfilename);
	}
	double t_end = cpuTime();
	out.open(logfilename, ios::app);
	out << "(DLX_new)Total time required to process the set is " << t_end - t_start << " seconds" << endl;
	out.close();
}

void free_dlx(vector<DLX_column*> &elements) {
	for (int i = 0; i < elements.size(); i++) {
		elements[i]->Column = NULL;
		elements[i]->Up = NULL;
		elements[i]->Down = NULL;
		elements[i]->Left = NULL;
		elements[i]->Right = NULL;
		DLX_column *t = elements[i];
		delete t;
	}
	elements.clear();
}


void generate_permutations_masked(int n, vector<vector<int>> &perm, vector<vector<int>> mask_LS, bool diag) {
	vector<vector<int>> MTV(10, vector<int>(10));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			MTV[i][j] = -1;
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (mask_LS[i][j] != -1) {
				MTV[mask_LS[i][j]][i] = j;
			}
		}
	}
	//here we need to make rearrangements if mask is split in several parts.
	//if it starts at the beginning as is single block then its fine.
	vector<int> ml;
	for (int i = 0; i < n; i++) {
		vector<int> seed_i;
		vector<bool> l(n);
		for (int j = 0; j < n; j++) {
			l[j] = false;
		}
		for (int j = 0; j < n; j++) {
			if (MTV[i][j] != -1) {
				seed_i.push_back(MTV[i][j]);
				l[MTV[i][j]] = true;
			}
		}
		int k = seed_i.size();
		for (int j = 0; j < n; j++) {
			if (l[j] == false) { seed_i.push_back(j); }
		}

		do {
			bool acc = true;
			if (diag == true) {
				int md = 0;
				int ad = 0;
				for (int j = 0; j < n; j++) {
					if (seed_i[j] == j) { md++; }
					if (seed_i[j] == n - j - 1) { ad++; }
				}
				if ((md != 1) || (ad != 1)) { acc = false; }
			}
			if (acc == true) { perm.push_back(seed_i); }
		} while (std::next_permutation(seed_i.begin() + k, seed_i.end()));

	}
	cout << "Generated " << perm.size() << "permutations" << endl;
}


void generate_permutations_masked_rc1(int n, vector<vector<int>> &perm, vector<vector<int>> mask_LS, bool diag) {
	vector<vector<int>> MTV(10, vector<int>(10));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			MTV[i][j] = -1;
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (mask_LS[i][j] != -1) {
				MTV[mask_LS[i][j]][i] = j;
			}
		}
	}
	vector<vector<int>> pm10;
	generate_permutations(n, pm10, diag);
	vector<vector<int>> res;
	for (int i = 0; i < pm10.size(); i++) {
		for (int k = 0; k < MTV.size(); k++) {
			bool acc = true;
			for (int j = 0; j < pm10[i].size(); j++) {
				if ((pm10[i][j] != MTV[k][j]) && (MTV[k][j] != -1)) {
					acc = false;
					break;
				}
			}
			if (acc == true) {
				res.push_back(pm10[i]);
			}
		}
	}
	perm = res;
}



void generate_permutations_blocks(int n, vector<vector<char>> &perm, vector<int> &bs, bool diag) {
	//bs = blockstart indexes 
	vector<char> seed;
	for (int i = 0; i < n; i++) {
		seed.push_back(i);
		bs.push_back(-1);
	}

	do {
		bool acc = true;
		if (diag == true) {
			int md = 0;
			int ad = 0;
			for (int j = 0; j < n; j++) {
				if (seed[j] == j) { md++; }
				if (seed[j] == n - j - 1) { ad++; }
			}
			if ((md != 1) || (ad != 1)) { acc = false; }
		}
		if (acc == true) {
			perm.push_back(seed);
			if (bs[seed[0]] == -1) {
				bs[seed[0]] = perm.size() - 1;
			}
		}
	} while (std::next_permutation(seed.begin(), seed.end()));
	bs.push_back(perm.size() - 1);
	//	cout << "Generated " << perm.size() << "permutations" << endl;
}
void SEARCH_TO_DLX(DLX_column &root, vector<vector<char>> & tvset, vector<DLX_column*> & elements) {
	int dimension = tvset[0].size() / 2;
	root.Up = NULL;
	root.Down = NULL;
	root.Column = NULL;
	root.row_id = -1;
	root.size = -1;
	//	root.column_number= -1;
	elements.push_back(&root);
	vector<DLX_column *> columns;
	DLX_column * lastleft = &root;
	for (int i = 0; i < 3 * dimension* dimension; i++) {
		DLX_column *ct;
		ct = new (DLX_column);
		//	ct->column_number = i;
		ct->Down = ct;
		ct->Up = ct;
		ct->size = 0;
		ct->row_id = 0;
		ct->Column = ct;
		ct->Left = lastleft;
		lastleft->Right = ct;
		lastleft = ct;
		columns.push_back(ct);
		elements.push_back(ct);
	}
	lastleft->Right = &root;
	root.Left = lastleft;

	for (int i = 0; i < tvset.size(); i++) {
		vector<char> curtv = tvset[i];
		vector<DLX_column *> tvrow;
		//tv set
		for (int j = 0; j < dimension; j++) {
			DLX_column *ctve;
			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind
			int k = j*dimension + curtv[j];

			ctve->Column = columns[k];
			ctve->Column->size++;
			ctve->Down = columns[k];
			ctve->Up = columns[k]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);
		}
		//ls_row
		for (int j = 0; j < dimension; j++) {
			DLX_column *ctve;
			ctve = new (DLX_column);
			//int e = curtv[dimension + j]; // value of tv element
			//column number in which we puch 1 corresponding to this value.
			//for row it means that if we have e==k, then in first *dimension* columns the *1* will be in *k*-th
			int k = dimension*dimension + j*dimension + curtv[dimension + j];
			// for column? int k =2 * dimension*dimension* + curtv[j]*dimension + e;?
			ctve->Column = columns[k];
			ctve->Column->size++;
			ctve->Down = columns[k];
			ctve->Up = columns[k]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);
		}
		//ls_column
		for (int j = 0; j < dimension; j++) {
			DLX_column *ctve;
			ctve = new (DLX_column);
			//int e = curtv[dimension + j]; // value of tv element
			//column number in which we puch 1 corresponding to this value.
			//for row it means that if we have e==k, then in first *dimension* columns the *1* will be in *k*-th
			int k = 2 * dimension*dimension + curtv[j] * dimension + curtv[dimension + j];
			ctve->Column = columns[k];
			ctve->Column->size++;
			ctve->Down = columns[k];
			ctve->Up = columns[k]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i;
			//	ctve->column_number = k;
			ctve->size = -10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);
		}
		for (int j = 0; j < tvrow.size() - 1; j++) {
			tvrow[j]->Right = tvrow[j + 1];
			tvrow[j]->Right->Left = tvrow[j];
		}
		tvrow[tvrow.size() - 1]->Right = tvrow[0];
		tvrow[0]->Left = tvrow[tvrow.size() - 1];
	}
	DLX_column *pr = &root;

}





void Generate_DLS_masked(int n, bool diag, unsigned long long  limit, string logname, vector<vector<int>>mask) {
	vector<vector<int>> perm_diag;
	generate_permutations_masked(n, perm_diag, mask, diag);
	sort(perm_diag.begin(), perm_diag.end());

	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);

	int size_big = sizeof(elements) + elements.size() * sizeof(elements[0]);
	cout << "The size of elements vector is " << size_big << endl;

	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;
	double t1 = cpuTime();
	unsigned long long count = 0;
	bool count_only = true;
	search_limited(0, *root, ps, tvr, cont, limit, count_only, count);
	double t2 = cpuTime();

	cout << tvr.size() << " squares generated in " << t2 - t1 << " seconds" << endl;
	//vector<vector<int>>> SQUARES(10, vector<int>(10));	

	vector<vector<int>> pm;
	generate_permutations(n, pm, diag);
	sort(pm.begin(), pm.end());

	DLX_column *root_pm;
	root_pm = new (DLX_column);
	vector<DLX_column*> columns;
	vector<vector<DLX_column*>> rows;
	TVSET_TO_DLX_EXT(*root_pm, pm, columns, rows);

	TRT *r10;
	r10 = new(TRT);
	construct_TR_tree(pm, r10, diag);

	double sc_t1 = cpuTime();
	vector<vector<vector<int>>> SQUARES(tvr.size(), vector<vector<int>>(n, vector<int>(n)));
	construct_squares_from_tv_set(perm_diag, tvr, SQUARES);

	/*
	//temp
	for (int i = 0; i < SQUARES.size(); i++) {
	cout << "SQUARE " << i << "\n";
	for (int u = 0; u < SQUARES[i].size(); u++) {
	for (int v = 0;  v<SQUARES[i][u].size(); v++) {
	cout << SQUARES[i][u][v] << " ";
	}
	cout << endl;
	}


	}
	//temp

	double sc_t2 = cpuTime();

	cout << "Construction of all LS from transversals took " << sc_t2 - sc_t1 << " seconds\n";
	cout << "Number of squares before filtration " << SQUARES.size() << "\n";
	double sc_cf_t1 = cpuTime();
	vector<vector<int>> SQ;
	vector<vector<vector<int>>> F_sq;
	for (int i = 0; i < SQUARES.size(); i++) {
	bool t = false;
	SQ = construct_cf_limited(SQUARES[i], t);
	if (t == true) {
	F_sq.push_back(SQUARES[i]);
	}		//BACKTRACK
	}
	double sc_cf_t2 = cpuTime();

	cout << "Computing reduced canonical forms took " << sc_cf_t2 - sc_cf_t1 << " seconds,\n";
	cout << "Number of squares after filtration " << F_sq.size() << "\n";

	*/
	double sc_check_t1 = cpuTime();
	check_squares_DLX_EXT_tv(r10, tvr, pm, perm_diag, root_pm, columns, rows, diag, logname);
	double sc_check_t2 = cpuTime();

	cout << "Checking all squares took " << sc_check_t2 - sc_check_t1 << " seconds,\n";
	//check_squares_DLX(SQUARES, true, logname);
}

vector<vector<int>> setcover_enc(int n, vector<int> &tv_ind, vector<vector<int>> TVSet, bool verbose) {
	vector<vector<int>> Encoding(n*n,vector<int>(0));
	int cnt = 0;
	/*vector<vector<int>> LSMask(n, vector<int>(n));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			LSMask[i][j] = ++cnt;
		}
	}*/
	vector<int> tv_vars(tv_ind.size());
	for (int i = 0; i < tv_ind.size(); i++) {
		tv_vars[i] = ++cnt;
	}

	//vector<vector<vector<int>>> tv_inds_ls (n,vector<vector<int>>(10,vector<int>(0)));
	for (int i = 0; i < tv_ind.size(); i++) {
		for (int j = 0; j < TVSet[tv_ind[i]].size(); j++) {
			Encoding[j*n + TVSet[tv_ind[i]][j]].push_back(tv_vars[i]);
//			tv_inds_ls[j][TVSet[tv_ind[i]][j]].push_back(tv_vars[i]);
	//		tv_inds_ls[j][TVSet[tv_ind[i]][j]].push_back(tv_vars[i]);
		}
	}
	for (int i = 0; i < n*n; i++) {
		AMO_pairwise(Encoding[i], Encoding);
	}
	if (verbose==true){
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
			//	printvector(tv_inds_ls[i][j]);
			//	cout << endl;
				printvector(Encoding[i*n+j]);
				cout << endl;
				for (int k = 0; k < Encoding[i*n + j].size(); k++) {
					cout << "Transversal " << k << endl;
					printvector(TVSet[tv_ind[Encoding[i*n + j][k]-1]]);
					cout << endl;
				}
				cout << endl;
			//	Encoding.push_back(tv_inds_ls[i][j]);			
			}
		}
	}
	return Encoding;

}
vector<vector<int>> setcover_enc_rc1(int n, vector<vector<int>> &tvr, bool verbose) {
	vector<vector<int>> Encoding(n*n, vector<int>(0));
	int cnt = 0;
	/*vector<vector<int>> LSMask(n, vector<int>(n));
	for (int i = 0; i < n; i++) {
	for (int j = 0; j < n; j++) {
	LSMask[i][j] = ++cnt;
	}
	}*/
	vector<int> tv_vars(tvr.size());
	for (int i = 0; i < tvr.size(); i++) {
		tv_vars[i] = ++cnt;
	}

	//vector<vector<vector<int>>> tv_inds_ls (n,vector<vector<int>>(10,vector<int>(0)));
	for (int i = 0; i < tvr.size(); i++) {
		for (int j = 0; j < tvr[i].size(); j++) {
			Encoding[j*n + tvr[i][j]].push_back(tv_vars[i]);
			//			tv_inds_ls[j][TVSet[tv_ind[i]][j]].push_back(tv_vars[i]);
			//		tv_inds_ls[j][TVSet[tv_ind[i]][j]].push_back(tv_vars[i]);
		}
	}
	for (int i = 0; i < n*n; i++) {
		AMO_pairwise(Encoding[i], Encoding);
	}
	if (verbose == true) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				//	printvector(tv_inds_ls[i][j]);
				//	cout << endl;
				printvector(Encoding[i*n + j]);
				cout << endl;
				for (int k = 0; k < Encoding[i*n + j].size(); k++) {
					cout << "Transversal " << k << endl;
					printvector(tvr[Encoding[i*n + j][k] - 1]);
					cout << endl;
				}
				cout << endl;
				//	Encoding.push_back(tv_inds_ls[i][j]);			
			}
		}
	}
	return Encoding;

}




vector<vector<int>> tv_search_enc(int n, vector<vector<int>> &SQ, bool verbose) {
	vector<vector<int>> Encoding(3*n+2, vector<int>(0));
	int cnt = 0;
	
	for (auto i = 0; i < n; i++) {
		for (auto j = 0; j < n; j++) {
			Encoding[i].push_back(i*n + j + 1);
			Encoding[n+j].push_back(i*n + j + 1);
			Encoding[2*n + SQ[i][j]].push_back(i*n + j+1);
			if (i == j) {
				Encoding[3 * n].push_back(i*n + j+1);
			}
			if (i ==(n-j-1)) {
				Encoding[3 * n+1].push_back(i*n + j+1);
			}
		}
	}
	for (auto i = 0; i < 3*n+2; i++) {
		ALO(Encoding[i], Encoding);
		AMO_pairwise(Encoding[i], Encoding);
	}
	//return Encoding;
	minisat22_wrapper s;
	Minisat::Problem p = s.convert_to_problem(Encoding);
	vector<int> assumpts;
	vector<vector<int>> tvr;
	s.minisat_solve_find_tv(p, n, assumpts, tvr);
	Encoding.clear();
	for (auto i = 0; i < p.size(); i++) {
		delete p[i];
	}
	p.clear();
	return tvr;
}




void check_SAT(int n, vector<vector<int>> &SQ, TRT*& r, vector<vector<int>> &TVSET, string logfilename) {		
	vector<int> trm;
	vector<int> tmp(10);
	TV_check_TRT(r->firstchild,SQ, trm, tmp);
	vector<vector<int>> E = setcover_enc(n, trm, TVSET,false);
	minisat22_wrapper s;
	Minisat::Problem p = s.convert_to_problem(E);
	vector<int> ss;
	vector<int> assumpts;
	s.minisat_solve_noout(p, assumpts, ss);
	if (ss.size() > 0) {
		ofstream out;
		out.open(logfilename, ios::app);
		for (int i = 0; i < SQ.size(); i++) {
			for (int j = 0; j < SQ[i].size(); j++) {
				out << SQ[i][j] << " ";
			}
			out << endl;
		}
		out << "(SAT) Total: " << trm.size()<< " transversals" << endl;
		out << "Found 1 set of disjoint transversals:" << endl;
		for (int i = 0; i < trm.size(); i++) {
			if (ss[i] > 0) {
				out << trm[i] << " ";
			}
		}
		out << endl;
		out.close();		
	}
	E.clear();

	for (auto i = 0; i < p.size(); i++) {
		delete p[i];
	}
	p.clear();
}

void check_SAT_rc1(int n, vector<vector<int>> &SQ, string logfilename) {
	vector<vector<int>> trm = find_tv_dlx(n, SQ);
	vector<vector<int>> E = setcover_enc_rc1(n, trm, false);
	minisat22_wrapper s;
	Minisat::Problem p = s.convert_to_problem(E);
	vector<int> ss;
	vector<int> assumpts;
	s.minisat_solve_noout(p, assumpts, ss);
	if (ss.size() > 0) {
		ofstream out;
		out.open(logfilename, ios::app);
		for (int i = 0; i < SQ.size(); i++) {
			for (int j = 0; j < SQ[i].size(); j++) {
				out << SQ[i][j] << " ";
			}
			out << endl;
		}
		out << "(SAT) Total: " << trm.size() << " transversals" << endl;
		out << "Found 1 set of disjoint transversals:" << endl;
		for (int i = 0; i < trm.size(); i++) {
			if (ss[i] > 0) {
				out << i << " ";
			}
		}
		out << endl;
		out.close();
	}
	E.clear();
	for (auto i = 0; i < p.size(); i++) {
		delete p[i];
	}
	p.clear();

}

void check_SAT_rc2(int n, vector<vector<int>> &SQ, string logfilename) {
	vector<vector<int>> trm = tv_search_enc(10, SQ, false);
	vector<vector<int>> E = setcover_enc_rc1(n, trm, false);
	minisat22_wrapper s;
	Minisat::Problem p = s.convert_to_problem(E);
	vector<int> ss;
	vector<int> assumpts;
	s.minisat_solve_noout(p, assumpts, ss);
	if (ss.size() > 0) {
		ofstream out;
		out.open(logfilename, ios::app);
		for (int i = 0; i < SQ.size(); i++) {
			for (int j = 0; j < SQ[i].size(); j++) {
				out << SQ[i][j] << " ";
			}
			out << endl;
		}
		out << "(SAT) Total: " << trm.size() << " transversals" << endl;
		out << "Found 1 set of disjoint transversals:" << endl;
		for (int i = 0; i < trm.size(); i++) {
			if (ss[i] > 0) {
				out << i << " ";
			}
		}
		out << endl;
		out.close();
	}
	E.clear();
	for (auto i = 0; i < p.size(); i++) {
		delete p[i];
	}
	p.clear();
}



void Generate_DLS_masked_compare(int n, bool diag, unsigned long long limit, string logname, vector<vector<int>>mask) {
	vector<vector<int>> perm_diag;
	generate_permutations_masked_rc1(n, perm_diag, mask, diag);
	sort(perm_diag.begin(), perm_diag.end());

	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);
		
	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;
	double t1 = cpuTime();
	unsigned long long count = 0;
	bool count_only = false;
	search_limited(0, *root, ps, tvr, cont, limit, count_only, count);
	double t2 = cpuTime();

	cout << tvr.size() << " squares generated in " << t2 - t1 << " seconds" << endl;
	//vector<vector<int>>> SQUARES(10, vector<int>(10));	

	vector<vector<int>> pm;
	generate_permutations(n, pm, diag);
	sort(pm.begin(), pm.end());

	DLX_column *root_pm;
	root_pm = new (DLX_column);
	vector<DLX_column*> columns;
	vector<vector<DLX_column*>> rows;
	TVSET_TO_DLX_EXT(*root_pm, pm, columns, rows);

	TRT *r10;
	r10 = new(TRT);
	construct_TR_tree(pm, r10, diag);

	double sc_t1 = cpuTime();
	vector<vector<vector<int>>> SQUARES(tvr.size(), vector<vector<int>>(n, vector<int>(n)));
	construct_squares_from_tv_set(perm_diag, tvr, SQUARES);

	
	/*double alt_SAT_check0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_SAT_alternative(n, SQUARES[i], r10, pm, logname);
	}
	double alt_SAT_check1 = cpuTime();
	cout << "checking squares via alternative SAT finished\n";
	*/
	
	//finding transversals using DLX
	double DLXTVsearh_0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		//vector<vector<int>> trm = getTrans_mod(SQUARES[i], diag);
		vector<vector<int>> temp_r=find_tv_dlx(n, SQUARES[i]);
		temp_r.clear();
	}
	double DLXTVsearh_1 = cpuTime();
	cout << "finding transversals using DLX took " << DLXTVsearh_1- DLXTVsearh_0<<endl;
	
	
	double SATTVsearh_0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		//vector<vector<int>> trm = getTrans_mod(SQUARES[i], diag);
		vector<vector<int>> temp_r = tv_search_enc(10, SQUARES[i], false);
		temp_r.clear();
	}
	double SATTVsearh_1 = cpuTime();
	cout << "finding transversals using SAT took " << SATTVsearh_1 - SATTVsearh_0 << endl;
	

	//finding transversals using Belyaev alg
	double BTVsearh_0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		vector<vector<int>> trm = getTrans_mod(SQUARES[i], diag);
		trm.clear();
	}
	double BTVsearh_1 = cpuTime();
	cout << "finding transversals using Belyaev alg took " << BTVsearh_1 - BTVsearh_0 << endl;
	//finding transversals using TRT tree

	double TRTTVsearh_0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		vector<int> trm;
		vector<int> tmp(10);
		TV_check_TRT(r10->firstchild, SQUARES[i], trm, tmp);		
		trm.clear();
	}
	double TRTTVsearh_1 = cpuTime();
	cout << "Finding transversals using TRT algorithm took " << TRTTVsearh_1 - TRTTVsearh_0 << " seconds\n";

	double SAT_check0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_SAT_rc2(n, SQUARES[i], logname);
	}
	double SAT_check1 = cpuTime();
	cout << "checking squares via SAT finished\n";


	//checking squares with belyev algorithm
	double Belyaev_check0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {		
		check_belyaev(SQUARES[i], diag, logname);			
	}
	double Belyaev_check1 = cpuTime();
	cout << "checking squares with belayev algorithm finished\n";
	//checking squares with old DLX implementation

	

	double SAT_check0_rc1 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_SAT_rc1(n, SQUARES[i], logname);
	}
	double SAT_check1_rc1 = cpuTime();
	cout << "checking squares via SAT rc1 finished\n";



	double OLDDLX_check0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_dlx(SQUARES[i], diag, logname);
	}
	double OLDDLX_check1 = cpuTime();
	cout << "checking squares with old DLX algorithm finished\n";
	//checking squares with new DLX implementation
			


	double OLDDLX_check0_rc1 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_dlx_rc1(SQUARES[i], diag, logname);
	}
	double OLDDLX_check1_rc1 = cpuTime();
	cout << "checking squares with DLX_refresh algorithm finished\n";
	//checking squares with new DLX implementation


	//check_dlx_rc1
	double sc_check_t1 = cpuTime();
	check_squares_DLX_EXT_tv(r10, tvr, pm, perm_diag, root_pm, columns, rows, diag, logname);
	double sc_check_t2 = cpuTime();



	cout << "RESULTS\n";
	cout << "finding transversals using DLX took " << DLXTVsearh_1 - DLXTVsearh_0 << endl;
	cout << "finding transversals using SAT took " << SATTVsearh_1 - SATTVsearh_0 << endl;
	cout << "Finding transversals using Belyaev algorithm took " << BTVsearh_1 - BTVsearh_0 << " seconds\n";
	cout << "Finding transversals using TRT algorithm took " << TRTTVsearh_1 - TRTTVsearh_0 << " seconds\n";
	cout << "Checking all SQUARES using SAT took " << SAT_check1 - SAT_check0 << " seconds\n";
	cout << "Checking all SQUARES using SAT +DLX took " << SAT_check1_rc1 - SAT_check0_rc1 << " seconds\n";
	cout << "Checking all SQUARES using Belyaev algorithm took " << Belyaev_check1 - Belyaev_check0 << " seconds\n";
	cout << "Checking all SQUARES using old DLX algorithm took " << OLDDLX_check1 - OLDDLX_check0 << " seconds\n";
	cout << "Checking all SQUARES using DLX refresh algorithm took " << OLDDLX_check1_rc1 - OLDDLX_check0_rc1 << " seconds\n";
	cout << "Checking all SQUARES using new DLX algorithm took " << sc_check_t2 - sc_check_t1 << " seconds\n";

	ofstream out;
	out.open(logname,ios::app);
	out << "RESULTS\n";
	out << tvr.size() << " squares generated in " << t2 - t1 << " seconds" << endl;
	out << "finding transversals using DLX took " << DLXTVsearh_1 - DLXTVsearh_0 << endl;
	out << "finding transversals using SAT took " << SATTVsearh_1 - SATTVsearh_0 << endl;
	out << "Finding transversals using Belyaev algorithm took " << BTVsearh_1 - BTVsearh_0 << " seconds\n";
	out << "Finding transversals using TRT algorithm took " << TRTTVsearh_1 - TRTTVsearh_0 << " seconds\n";
	out << "Checking all SQUARES using SAT took " << SAT_check1 - SAT_check0 << " seconds\n";
	out << "Checking all SQUARES using SAT +DLX took " << SAT_check1_rc1 - SAT_check0_rc1 << " seconds\n";
	out << "Checking all SQUARES using Belyaev algorithm took " << Belyaev_check1 - Belyaev_check0 << " seconds\n";
	out << "Checking all SQUARES using old DLX algorithm took " << OLDDLX_check1 - OLDDLX_check0 << " seconds\n";
	out << "Checking all SQUARES using DLX refresh algorithm took " << OLDDLX_check1_rc1 - OLDDLX_check0_rc1 << " seconds\n";
	out << "Checking all SQUARES using new DLX algorithm took " << sc_check_t2 - sc_check_t1 << " seconds\n";
	out.close();

	//cout << "Checking all squares took " << sc_check_t2 - sc_check_t1 << " seconds,\n";
	//check_squares_DLX(SQUARES, true, logname);
}

bool isdiagls(int n, vector<vector<int>> &SQ) {
	bool b = true;
	for (auto i = 0; i < n; i++) {
		vector<int> r(n);
		//checking rows
		for (auto j = 0; j < n; j++) {
			r[SQ[i][j]]++;
			if (r[SQ[i][j]]>1) {
				b = false;
				return false;
			}
		}
		//checking columns
		vector<int> c(n);
		for (auto j = 0; j < n; j++) {
			c[SQ[j][i]]++;
			if (c[SQ[j][i]]>1) {
				b = false;
				return false;
			}
		}
	}
	vector<int> md(n);
	vector<int> ad(n);
	//checking diags;
	for (auto j = 0; j < n; j++) {
		md[SQ[j][j]]++;
		ad[SQ[j][n - j - 1]]++;
		if ((md[SQ[j][j]]>1)||(ad[SQ[j][n - j - 1]]>1)){
			b = false;
			return false;
		}
	}
	return b;
}
void Generate_DLS_masked_crosscheck(int n, bool diag, unsigned long long limit, string logname, vector<vector<int>>mask) {
	vector<vector<int>> perm_diag;
	generate_permutations_masked_rc1(n, perm_diag, mask, diag);
	sort(perm_diag.begin(), perm_diag.end());

	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);

	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;
	double t1 = cpuTime();
	unsigned long long count = 0;
	bool count_only = false;
	search_limited(0, *root, ps, tvr, cont, limit, count_only, count);
	double t2 = cpuTime();

	cout << tvr.size() << " squares generated in " << t2 - t1 << " seconds" << endl;
	//vector<vector<int>>> SQUARES(10, vector<int>(10));	

	vector<vector<int>> pm;
	generate_permutations(n, pm, diag);
	sort(pm.begin(), pm.end());

	DLX_column *root_pm;
	root_pm = new (DLX_column);
	vector<DLX_column*> columns;
	vector<vector<DLX_column*>> rows;
	TVSET_TO_DLX_EXT(*root_pm, pm, columns, rows);

	TRT *r10;
	r10 = new(TRT);
	construct_TR_tree(pm, r10, diag);

	double sc_t1 = cpuTime();
	vector<vector<vector<int>>> SQUARES(tvr.size(), vector<vector<int>>(n, vector<int>(n)));
	construct_squares_from_tv_set(perm_diag, tvr, SQUARES);
	for (auto i = 0; i < SQUARES.size(); i++) {
		if (isdiagls(n, SQUARES[i]) == false) {
			cout << "Non-diagonal LS found @ " << i << endl;
		}
		vector<vector<int>> TVDLX = find_tv_dlx(n, SQUARES[i]);
		vector<vector<int>> TVSAT = tv_search_enc(10, SQUARES[i], false);
		vector<vector<int>> TVBEL= getTrans_mod(SQUARES[i], diag);
		vector<int> trm;
		vector<int> tmp(10);
		TV_check_TRT(r10->firstchild, SQUARES[i], trm, tmp);
		vector<vector<int>> TVTRT;
		for (auto j = 0; j < trm.size(); j++) {
			TVTRT.push_back(pm[trm[j]]);
		}

		sort(TVDLX.begin(), TVDLX.end());
		sort(TVSAT.begin(), TVSAT.end());
		sort(TVBEL.begin(), TVBEL.end());
		sort(TVTRT.begin(), TVTRT.end());

		if ((TVDLX.size() != TVSAT.size())|| (TVSAT.size()!=TVBEL.size())||(TVBEL.size()!=TVTRT.size())) {
			cout << "Error, TV vector size mismatch\n";
		}
		else {
			for (auto j = 0; j < TVDLX.size(); j++) {
				if ((TVDLX[j] != TVSAT[j]) || (TVSAT[j] != TVBEL[j]) || (TVBEL[j] != TVTRT[j])) {
					cout << "Error, different transversals @TV " << j << endl;
				}
			}

		}
		if (i % 100 == 0) {
			cout << "Processed " << i << " squares\n";
		}
	}
	/*
//checking squares with belyev algorithm
	double Belyaev_check0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_belyaev(SQUARES[i], diag, logname);
	}
	double Belyaev_check1 = cpuTime();
	cout << "checking squares with belayev algorithm finished\n";
	//checking squares with old DLX implementation



	double SAT_check0_rc1 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_SAT_rc1(n, SQUARES[i], logname);
	}
	double SAT_check1_rc1 = cpuTime();
	cout << "checking squares via SAT rc1 finished\n";



	double OLDDLX_check0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_dlx(SQUARES[i], diag, logname);
	}
	double OLDDLX_check1 = cpuTime();
	cout << "checking squares with old DLX algorithm finished\n";
	//checking squares with new DLX implementation



	double OLDDLX_check0_rc1 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_dlx_rc1(SQUARES[i], diag, logname);
	}
	double OLDDLX_check1_rc1 = cpuTime();
	cout << "checking squares with DLX_refresh algorithm finished\n";
	//checking squares with new DLX implementation


	//check_dlx_rc1
	double sc_check_t1 = cpuTime();
	check_squares_DLX_EXT_tv(r10, tvr, pm, perm_diag, root_pm, columns, rows, diag, logname);
	double sc_check_t2 = cpuTime();



	cout << "RESULTS\n";
	cout << "finding transversals using DLX took " << DLXTVsearh_1 - DLXTVsearh_0 << endl;
	cout << "finding transversals using SAT took " << SATTVsearh_1 - SATTVsearh_0 << endl;
	cout << "Finding transversals using Belyaev algorithm took " << BTVsearh_1 - BTVsearh_0 << " seconds\n";
	cout << "Finding transversals using TRT algorithm took " << TRTTVsearh_1 - TRTTVsearh_0 << " seconds\n";
	cout << "Checking all SQUARES using SAT took " << SAT_check1 - SAT_check0 << " seconds\n";
	cout << "Checking all SQUARES using SAT +DLX took " << SAT_check1_rc1 - SAT_check0_rc1 << " seconds\n";
	cout << "Checking all SQUARES using Belyaev algorithm took " << Belyaev_check1 - Belyaev_check0 << " seconds\n";
	cout << "Checking all SQUARES using old DLX algorithm took " << OLDDLX_check1 - OLDDLX_check0 << " seconds\n";
	cout << "Checking all SQUARES using DLX refresh algorithm took " << OLDDLX_check1_rc1 - OLDDLX_check0_rc1 << " seconds\n";
	cout << "Checking all SQUARES using new DLX algorithm took " << sc_check_t2 - sc_check_t1 << " seconds\n";

	ofstream out;
	out.open("D:\\LSTests\\Compare_res.log");
	out << "RESULTS\n";
	out << tvr.size() << " squares generated in " << t2 - t1 << " seconds" << endl;
	out << "finding transversals using DLX took " << DLXTVsearh_1 - DLXTVsearh_0 << endl;
	out << "finding transversals using SAT took " << SATTVsearh_1 - SATTVsearh_0 << endl;
	out << "Finding transversals using Belyaev algorithm took " << BTVsearh_1 - BTVsearh_0 << " seconds\n";
	out << "Finding transversals using TRT algorithm took " << TRTTVsearh_1 - TRTTVsearh_0 << " seconds\n";
	out << "Checking all SQUARES using SAT took " << SAT_check1 - SAT_check0 << " seconds\n";
	out << "Checking all SQUARES using SAT +DLX took " << SAT_check1_rc1 - SAT_check0_rc1 << " seconds\n";
	out << "Checking all SQUARES using Belyaev algorithm took " << Belyaev_check1 - Belyaev_check0 << " seconds\n";
	out << "Checking all SQUARES using old DLX algorithm took " << OLDDLX_check1 - OLDDLX_check0 << " seconds\n";
	out << "Checking all SQUARES using DLX refresh algorithm took " << OLDDLX_check1_rc1 - OLDDLX_check0_rc1 << " seconds\n";
	out << "Checking all SQUARES using new DLX algorithm took " << sc_check_t2 - sc_check_t1 << " seconds\n";
	out.close();
	*/
	//cout << "Checking all squares took " << sc_check_t2 - sc_check_t1 << " seconds,\n";
	//check_squares_DLX(SQUARES, true, logname);
}




uint64_t Generate_DLS_masked_nocheck(int n, bool diag, unsigned long long limit, bool count_only, bool verbosity, string logname, vector<vector<int>>mask, vector<vector<vector<int>>> &SQUARES) {
	vector<vector<int>> perm_diag;
	generate_permutations_masked_rc1(n, perm_diag, mask, diag);
	sort(perm_diag.begin(), perm_diag.end());

	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);

	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;
	double t1 = cpuTime();
	unsigned long long count = 0;
	search_limited(0, *root, ps, tvr, cont, limit, count_only, count);
	double t2 = cpuTime();

	cout << count << " squares generated in " << t2 - t1 << " seconds" << endl;
	ofstream out;
	out.open(logname);
	out << count << " squares generated in " << t2 - t1 << " seconds" << endl;
	out.close();

	//vector<vector<int>>> SQUARES(10, vector<int>(10));	
	/*
	void construct_squares_from_tv_set(vector<vector<int>>&tv_set, vector<vector<int>> &tv_index_sets, vector<vector<vector<int>>> &SQUARES) {
	for (int i = 0; i < tv_index_sets.size(); i++) {
	construct_square_from_tv(tv_set, tv_index_sets[i], SQUARES[i]);
	}
	}
	*/
	if (count_only == false) {
		construct_squares_from_tv_set(perm_diag, tvr, SQUARES);
		if (verbosity == true) {
			ofstream out;
			out.open(logname,ios::app);
			for (int i = 0; i < SQUARES.size(); i++) {
				for (int u = 0; u < SQUARES[i].size(); u++) {
					for (int v = 0; v < SQUARES[i].size(); v++) {
						out << SQUARES[i][u][v] << " ";
					}
					out << endl;
				}
				out << endl;
			}
			out.close();
		}
	}
	return count;
}


void Generate_DLS_masked_DLXrefresh(int n, bool diag, unsigned long long limit, string logname, vector<vector<int>>mask) {
	vector<vector<int>> perm_diag;
	generate_permutations_masked_rc1(n, perm_diag, mask, diag);
	sort(perm_diag.begin(), perm_diag.end());

	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);

	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;
	double t1 = cpuTime();
	unsigned long long count = 0;
	bool count_only = false;
	search_limited(0, *root, ps, tvr, cont, limit, count_only, count);
	double t2 = cpuTime();

	cout << tvr.size() << " squares generated in " << t2 - t1 << " seconds" << endl;
	//vector<vector<int>>> SQUARES(10, vector<int>(10));	


	double sc_t1 = cpuTime();
	vector<vector<vector<int>>> SQUARES(tvr.size(), vector<vector<int>>(n, vector<int>(n)));
	construct_squares_from_tv_set(perm_diag, tvr, SQUARES);

	double OLDDLX_check0_rc1 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_dlx_rc1(SQUARES[i], diag, logname);
	}
	double OLDDLX_check1_rc1 = cpuTime();
	cout << "checking squares with DLX_refresh algorithm finished\n";
	//checking squares with new DLX implementation


	//check_dlx_rc1
	


	cout << "RESULTS\n";
	
	cout << "Checking all SQUARES using DLX refresh algorithm took " << OLDDLX_check1_rc1 - OLDDLX_check0_rc1 << " seconds\n";
	

	//cout << "Checking all squares took " << sc_check_t2 - sc_check_t1 << " seconds,\n";
	//check_squares_DLX(SQUARES, true, logname);
}

void Generate_DLS(int dimension, unsigned long long limit, string logname) {
	int n = dimension;
	vector<vector<int>> perm_diag;
	generate_permutations(n, perm_diag, true);

	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);

	int size_big = sizeof(elements) + elements.size() * sizeof(elements[0]);
	cout << "The size of elements vector is " << size_big << endl;

	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;
	double t1 = cpuTime();
	unsigned long long count = 0;
	bool count_only = true;
	search_limited(0, *root, ps, tvr, cont, limit, count_only, count);
	double t2 = cpuTime();
	cout << limit << " squares generated in " << t2 - t1 << " seconds" << endl;
	//vector<vector<int>>> SQUARES(10, vector<int>(10));	
	vector<vector<vector<int>>> SQUARES;
	for (int i = 0; i < tvr.size(); i++) {
		(dimension, vector<int>(dimension));
		vector<vector<int>> SQ(dimension, vector<int>(dimension));
		SQUARES.push_back(SQ);
	}

	construct_squares_from_tv_set(perm_diag, tvr, SQUARES);
	check_squares_DLX(SQUARES, true, logname);

}
void filter_TV(vector<vector<int>> &TVSET, vector<vector<int>> &LS_mask) {
	cout << "Size before filtering " << TVSET.size() << endl;
	vector<vector<int>> TVSET_new;
	int n = TVSET[0].size();

	for (int i = 0; i < TVSET.size(); i++) {
		vector<int> tmp(10);
		bool a = true;
		for (int u = 0; u < n; u++) {
			if (LS_mask[u][TVSET[i][u]] >= 0) {
				tmp[LS_mask[u][TVSET[i][u]]]++;
				if (tmp[LS_mask[u][TVSET[i][u]]]>1) {
					a = false;
					TVSET_new.push_back(TVSET[i]);
					break;
				}
			}
		}
	}
	TVSET = TVSET_new;
	cout << "Size after filtering " << TVSET_new.size() << endl;
}

//new code





//new code end

void print_sq(vector<vector<int>> &SQ) {
	cout << endl;
	for (int i = 0; i < SQ.size(); i++) {
		for (int j = 0; j < SQ[i].size(); j++) {
			cout << SQ[i][j] << " ";
		}
		cout << endl;
	}
}

vector<vector<int>> loadcnffromfile(int &nvars, string filename) {
	vector<vector<int>> res;
	int n = 0;
	ifstream myfile;
	bool r = false;
	myfile.open(filename);
	string s;
	while (myfile.good()) {
		getline(myfile, s);
		if ((s[0] == 'p') || (s[0] == 'c')) {
			//skip
		}
		else {
			vector<int> res_i;
			int k = 0;
			for (int t = 0; t < s.length(); t++) {
				if (s[t] == ' ') {
					string tmp = s.substr(k, t - k);
					int a = strtoi(tmp);
					if (abs(a) > n) {
						n = abs(a);
					}
					if (a != 0) {
						res_i.push_back(a);
					}
					k = t;
				}
			}
			if (res_i.size() != 0) {
				res.push_back(res_i);
			}
		}
	}
	myfile.close();
	nvars = n;
	return res;
}

vector<int> basic_filter(vector<vector<int>> &pm, vector<int> &ind, vector<int> & fil) {
	vector<int> res;
	for (int i = 0; i < ind.size(); i++) {
		bool acc = true;
		for (int j = 0; j < fil.size(); j++) {
			if (fil[j] == pm[ind[i]][j]) { acc = false;  break; }
		}
		if (acc == true) res.push_back(ind[i]);
	}
	return res;
}

unsigned __int64 count_LS(vector<vector<int>> &pm, vector<int> &ind, vector<vector<int>> & PLS, int depth) {
	if (ind.size() == 0) return 0;
	vector<int> res;
	vector<int> filter_on = PLS[PLS.size() - 1];

	vector<int> filter_res1 = basic_filter(pm, ind, filter_on);
	vector<int> md(PLS[0].size());
	vector<int> ad(PLS[0].size());

	//let only diagonal remain
	for (int i = 0; i < PLS.size(); i++) {
		md[PLS[i][i]]++;
		ad[PLS[i][PLS[0].size() - i - 1]]++;
	}
	int k = PLS.size();

	for (int i = 0; i < filter_res1.size(); i++) {
		vector<int> tmp = pm[filter_res1[i]];
		//	for (int j = 0; j < tmp.size(); j++) {
		//	cout << tmp[j] << " ";
		//}
		//		cout << "MD: " << tmp[k] << ", AD: " << tmp[PLS[0].size() - k - 1] << endl;

		if ((md[tmp[k]] == 0) && (ad[tmp[PLS[0].size() - k - 1]] == 0)) {

			res.push_back(i);
		}
	}
	unsigned __int64 sum = 0;
	if (depth > 0) {

		for (int i = 0; i < res.size(); i++) {
			if ((i>0) && (i % 100 == 0)) {
				cout << "Depth " << depth << ", processed " << i << " out of " << res.size() << " variants, current sum " << sum << "\n";
			}
			PLS.push_back(pm[res[i]]);
			sum += count_LS(pm, filter_res1, PLS, depth - 1);
			PLS.pop_back();
		}
	}
	else {
		sum = res.size();
	}
	return sum;
}

void inc_ii(int &ii, int &i, int &j, int &dimension, vector<vector<int>> &SQ, vector<vector<int>> &R, vector<vector<int>>&C) {
	if (i == dimension) {
		i = dimension - 1;
		j = dimension - 1;
		R[i][SQ[i][j]]--;
		C[j][SQ[i][j]]--;
		ii = SQ[i][j];
	}
	ii++;
	while (ii >= dimension) {
		if (j > 0) {
			j--;
			ii = SQ[i][j];
			R[i][ii]--;
			C[j][ii]--;
			ii++;
		}
		else {
			i--;
			j = dimension - 1;
			ii = SQ[i][j];
			R[i][ii]--;
			C[j][ii]--;
			ii++;
		}
	}
}

void gen_LS(int dimension, bool count_only) {
	vector<vector<vector<int>>> res;
	vector<vector<int>> SQ(dimension, vector<int>(dimension));
	vector<vector<int>> ROWS(dimension, vector<int>(dimension));
	vector<vector<int>> COLUMNS(dimension, vector<int>(dimension));
	int d = dimension;

	for (int l = 0; l < dimension; l++) {
		SQ[0][l] = l;
		ROWS[0][l]++;
		COLUMNS[l][l]++;
	}
	int cnt = 0;
	int i = 1;
	int j = 0;
	int ii = 0;
	while (i > 0) {
		if (i == dimension) {
			cnt++;
			res.push_back(SQ);
			cout << "SQUARE # " << cnt - 1 << "\n";
			for (int u = 0; u < dimension; u++) {
				for (int v = 0; v < dimension; v++) {
					cout << SQ[u][v] << " ";
				}
				cout << "\n";
			}
			inc_ii(ii, i, j, dimension, SQ, ROWS, COLUMNS);

		}

		if ((ROWS[i][ii] == 0) && (COLUMNS[j][ii] == 0)) {
			SQ[i][j] = ii;
			ROWS[i][ii]++;
			COLUMNS[j][ii]++;
			if (j < dimension - 1) {
				j++;
			}
			else
			{
				i++;
				j = 0;
			}
			ii = 0;
		}
		else {
			inc_ii(ii, i, j, dimension, SQ, ROWS, COLUMNS);
		}

	}

}


void inc_ii_D(int &ii, int &i, int &j, int &dimension, vector<vector<int>> &SQ, vector<vector<int>> &R, vector<vector<int>>&C, vector<int> &AD, vector<int> &MD) {
	if (i == dimension) {
		i = dimension - 1;
		j = dimension - 1;
		R[i][SQ[i][j]]--;
		C[j][SQ[i][j]]--;
		MD[SQ[i][j]]--;
		ii = SQ[i][j];
	}
	ii++;
	while (ii >= dimension) {
		if (j > 0) {
			j--;
			ii = SQ[i][j];
			R[i][ii]--;
			C[j][ii]--;
			if (i == j) { MD[SQ[i][j]]--; }
			if (j == dimension - i - 1) { AD[SQ[i][j]]--; }
			ii++;
		}
		else {
			i--;
			j = dimension - 1;
			ii = SQ[i][j];
			R[i][ii]--;
			C[j][ii]--;
			if (i == j) { MD[SQ[i][j]]--; }
			if (j == dimension - i - 1) { AD[SQ[i][j]]--; }
			ii++;
		}
	}
}


int gen_LSD(int dimension, bool count_only, bool verbosity) {
	vector<vector<vector<int>>> res;
	vector<vector<int>> SQ(dimension, vector<int>(dimension));
	vector<vector<int>> ROWS(dimension, vector<int>(dimension));
	vector<vector<int>> COLUMNS(dimension, vector<int>(dimension));
	vector<int> MD(dimension);
	vector<int> AD(dimension);
	int d = dimension;

	for (int l = 0; l < dimension; l++) {
		SQ[0][l] = l;
		ROWS[0][l]++;
		COLUMNS[l][l]++;
	}
	MD[0]++;
	AD[d - 1]++;
	int cnt = 0;
	int i = 1;
	int j = 0;
	int ii = 0;
	while (i > 0) {
		if (i == dimension) {
			cnt++;
			if (count_only == false) {
				res.push_back(SQ);
			}
			if (verbosity == true) {
				cout << "SQUARE # " << cnt - 1 << "\n";
				for (int u = 0; u < dimension; u++) {
					for (int v = 0; v < dimension; v++) {
						cout << SQ[u][v] << " ";
					}
					cout << "\n";
				}
			}
			inc_ii_D(ii, i, j, dimension, SQ, ROWS, COLUMNS, AD, MD);

		}
		bool MDE = (i != j) || ((i == j) && (MD[ii] == 0));
		bool ADE = (j != d - i - 1) || ((j == d - i - 1) && (AD[ii] == 0));


		if ((ROWS[i][ii] == 0) && (COLUMNS[j][ii] == 0) && MDE && ADE) {
			SQ[i][j] = ii;
			ROWS[i][ii]++;
			COLUMNS[j][ii]++;
			if (i == j) { MD[SQ[i][j]]++; }
			if (j == d - i - 1) { AD[SQ[i][j]]++; }

			if (j < dimension - 1) {
				j++;
			}
			else
			{
				i++;
				j = 0;
			}
			ii = 0;
		}
		else {
			inc_ii_D(ii, i, j, dimension, SQ, ROWS, COLUMNS, AD, MD);
		}

	}
	return cnt;
}


int count_PLS(int dimension, int n_of_rows) {
	vector<vector<int>> perm;
	generate_permutations(dimension, perm, false);

	vector<vector<int>> cur_PLS(1, vector<int>(dimension));
	vector<int> first_row;
	for (int i = 0; i < dimension; i++) first_row.push_back(i);
	cur_PLS[0] = first_row;

	vector<int> ind(perm.size());
	for (int i = 0; i < perm.size(); i++) { ind[i] = i; }

	unsigned __int64 r = count_LS(perm, ind, cur_PLS, n_of_rows - 1);
	//	cout << "counted " << r << " variants\n";
	return r;
}

vector<vector<int>> compute_masked_LS(vector<vector<int>> &LS, vector<vector<int>> &MASK) {
	vector<vector<int>> res(LS);
	if (MASK.size() != LS.size()) {
		std::cout << "LS and MASK sizes dont match \n";
	}
	for (int i = 0; i < MASK.size(); i++) {
		if (MASK[i].size() != LS[i].size()) {
			std::cout << "LS and MASK sizes dont match @ " << i << "\n";
		}
		for (int j = 0; j < MASK[i].size(); j++) {
			if (MASK[i][j] == -1) {
				res[i][j] = -1;
			}
		}
	}
	return res;
}

vector<vector<int>> compute_masked_LS(vector<vector<int>> &LS, int k) {
	vector<vector<int>> res(LS);
	int n = LS.size();
	for (int i = k; i < n*n; i++) {
		res[i / n][i%n] = -1;
	}
	return res;
}



void ineq_char(int v, vector<int> lp, vector<int> rp1, vector<int> rp2, vector<vector<int>> & E) {
	for (int i = 0; i < rp1.size(); i++) {
		var_eq_and(lp[i], -rp1[i], -rp2[i], E);
	}
	var_eq_or(v, lp, E);
}

vector<vector<int>> tv_find_sat_enc(int n, bool diag) {
	vector<vector<int>> Encoding;
	vector<vector<int>> pm;
	generate_permutations(n, pm, diag);
	int cnt_vars = 1;
	vector<int> tv_vars(pm.size());
	for (int i = 0; i < pm.size(); i++) {
		tv_vars[i] = cnt_vars++;
	}
	vector<vector<vector<int>>> ls_vars(n, vector<vector<int>>(n, vector<int>(n)));
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < n; k++){
				ls_vars[i][j][k]=cnt_vars++;
			}
		}
	}
	int m = n*n;
	vector<vector<int>> aux_vars(m, vector<int>(m));
	for (int i = 0; i < m; i++) {
		for (int j = i+1; j < m; j++) {
			aux_vars[i][j] = cnt_vars++;
		}
	}

	for (int i = 0; i < n*n; i++) {
		for (int j = i+1; j < n*n; j++) {
				vector<int> t;
				for (int u = 0; u < n; u++) {
					t.push_back(cnt_vars++);
				}				
				ineq_char(aux_vars[i][j], t, ls_vars[i%n][i/n], ls_vars[j%n][j/n], Encoding);			
		}
	}

	/*for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int i1 = i + 1; i1 < n; i1++) {
				for (int j1 = 0; j1 < n; j1++) {
					vector<int> t;
					for (int u = 0; u < n; u++) {
						t.push_back(cnt_vars++);
					}
					cout << i << " " << j << " " << i1 << " " << j1 << endl;
					ineq_char(aux_vars[i*n + j][i1*n + j1], t, ls_vars[i][j], ls_vars[i1][j1],Encoding);
				}
			}
		}
	}*/

	vector<vector<int>> tv_vects;

	for (int i = 0; i < pm.size(); i++) {
		vector<int> t;
	//	printvector(pm[i]);
		for (int j = 0; j < pm[i].size()-1;  j++) {
			for (int k = j + 1; k < n;  k++) {
				t.push_back(aux_vars[j*n + pm[i][j]][(k)*n + pm[i][k]]);
				//cout << j*n + pm[i][j] << " " << " " << (k)*n + pm[i][k] << endl;
			}
		}
		tv_vects.push_back(t);
	}
	for (int i = 0; i < tv_vars.size(); i++) {
		var_eq_and(tv_vars[i], tv_vects[i], Encoding);
	}
	cout << Encoding.size() << endl;

	string cnf_name = "D:\\LStests\\Test_tv_find" + inttostr(n) + ".cnf";
	ofstream out;
	out.open(cnf_name);
	for (int i = 0; i < Encoding.size(); i++) {
		for (int j = 0; j < Encoding[i].size(); j++) {
			out << Encoding[i][j] << " ";
		}
		out << "0\n";
	}
	out.close();
	return Encoding;
}

bool check3(vector<int> &r0, vector<int>&r1, vector<int> r2) {
	/*cout << "Checking \n";
	printvector(r0);
	cout << endl;
	printvector(r1);
	cout << endl;
	printvector(r2);
	cout << endl;
*/
	int n = r0.size();
	bool r = true;
	for (int i = 0; i < n; i++) {
		if ((r0[i] == r1[i]) || (r0[i] == r2[i]) || (r1[i] == r2[i])) {
			r = false;
			return r;
		}
	}
	if ((r0[0] == r1[1]) || (r0[0] == r2[2]) || (r1[1] == r2[2])) {
		r = false;
		return r;
	}
	if ((r0[n-1] == r1[n-2]) || (r0[n-1] == r2[n-3]) || (r1[n-2] == r2[n-3])) {
		r = false;
		return r;
	}
	return r;
}

bool check4(vector<int> &r0, vector<int>&r1, vector<int> r2, vector<int> r3) {
	/*cout << "Checking \n";
	printvector(r0);
	cout << endl;
	printvector(r1);
	cout << endl;
	printvector(r2);
	cout << endl;
	*/
	int n = r0.size();
	bool r = true;
	for (int i = 0; i < n; i++) {
		if ((r0[i] == r1[i]) || (r0[i] == r2[i]) || (r1[i] == r2[i])||(r0[i]==r3[i]) || (r1[i] == r3[i]) || (r2[i] == r3[i])) {
			r = false;
			return r;
		}
	}
	if ((r0[0] == r1[1]) || (r0[0] == r2[2]) || (r1[1] == r2[2]) || (r0[0] == r3[3]) || (r1[1] == r3[3]) || (r2[2] == r3[3])) {
		r = false;
		return r;
	}
	if ((r0[n - 1] == r1[n - 2]) || (r0[n - 1] == r2[n - 3]) || (r1[n - 2] == r2[n - 3]) ||
		(r0[n - 1] == r3[n - 4]) || (r1[n - 2] == r3[n - 4]) || (r2[n - 3] == r3[n - 4])) {
		r = false;
		return r;
	}
	return r;
}

void generate_3_rows_rand(int n) {
	vector<vector<int>> pm;
	generate_permutations(n, pm, false);

	vector<int> r0(n);
	vector<int> r1(n);
	vector<int> r2(n);
	r0 = pm[0];
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist_pm(0, pm.size()-1);
	unsigned long int t = 0;
	bool f = false;
	while (!f) {
		t++;
		int k1 = dist_pm(rng);
		//cout << k1 << endl;
		int k2 = dist_pm(rng);
	//	cout << k2 << endl;
		r1 = pm[k1];
		r2 = pm[k2];
		bool f = check3(r0, r1, r2);
		if (t % 10000000 == 0) { cout << t << endl; }
	}

	cout << "Found random r3" << endl;
	printvector(r0);
	cout << endl;
	printvector(r1);
	cout << endl;
	printvector(r2);
	cout << endl;

}

void generate_3r_rnd_rc1(int n, int k) {
	vector<vector<int>> pm1;
	generate_permutations(n, pm1, false);
	vector<int> r0(n);
	vector<int> r1(n);
	vector<int> r2(n);
	r0 = pm1[0];

	vector<vector<int>> pm2;
	for (int i = 0; i < pm1.size(); i++) {
		vector<int> t = pm1[i];
		bool b = true;
		for (int j = 0; j < n; j++) {
			if (t[j] == r0[j]) {
				b = false;
				break;
			}
		}
		if ((r0[0] == t[1]) || (r0[n - 1] == t[n - 2])) {
			b = false;
		}
		if (b == true) {
			pm2.push_back(t);
		}
	}
	for (int i = 0; i < k; i++) {
		std::mt19937 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist_pm2(0, pm2.size() - 1);
		int k1 = dist_pm2(rng);
		r1 = pm2[k1];

		vector<vector<int>> pm3;

		for (int u = 0; u < pm1.size(); u++) {
			vector<int> t = pm1[u];
			bool b = true;
			for (int j = 0; j < n; j++) {
				if ((t[j] == r0[j])||(t[j]==r1[j])) {
					b = false;
					break;
				}
			}
			if ((r0[0] == t[2]) || (r0[n - 1] == t[n - 3])||(r1[1]==t[2])||(r1[n-2]==t[n-3])) {				
				b = false;
			}
			if (b == true) {
				pm3.push_back(t);
			}
		}

		std::mt19937 rng2;
		rng2.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist_pm3(0, pm3.size() - 1);
		int k2 = dist_pm3(rng2);
		r2 = pm3[k2];
		if (check3(r0, r1, r2) == true) {
			cout << "Variant " << i << endl;
			printvector(r0);
			cout << endl;
			printvector(r1);
			cout << endl;
			printvector(r2);
			cout << endl << endl;
		}


	}
	
}

void generate_4r_rnd_rc1(int n, int k,vector<vector<vector<int>>> &SQmasks) {
	SQmasks.clear();
	vector<vector<int>> pm1;
	generate_permutations(n, pm1, false);
	vector<int> r0(n);
	vector<int> r1(n);
	vector<int> r2(n);
	vector<int> r3(n);
	r0 = pm1[0];

	vector<vector<int>> pm2;
	for (int i = 0; i < pm1.size(); i++) {
		vector<int> t = pm1[i];
		bool b = true;
		for (int j = 0; j < n; j++) {
			if (t[j] == r0[j]) {
				b = false;
				break;
			}
		}
		if ((r0[0] == t[1]) || (r0[n - 1] == t[n - 2])) {
			b = false;
		}
		if (b == true) {
			pm2.push_back(t);
		}
	}

	for (int i = 0; i < k; i++) {
		std::mt19937 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist_pm2(0, pm2.size() - 1);
		int k1 = dist_pm2(rng);
		r1 = pm2[k1];

		vector<vector<int>> pm3;

		for (int u = 0; u < pm1.size(); u++) {
			vector<int> t = pm1[u];
			bool b = true;
			for (int j = 0; j < n; j++) {
				if ((t[j] == r0[j]) || (t[j] == r1[j])) {
					b = false;
					break;
				}
			}
			if ((r0[0] == t[2]) || (r0[n - 1] == t[n - 3]) || (r1[1] == t[2]) || (r1[n - 2] == t[n - 3])) {
				b = false;
			}
			if (b == true) {
				pm3.push_back(t);
			}
		}

		std::mt19937 rng2;
		rng2.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist_pm3(0, pm3.size() - 1);
		int k2 = dist_pm3(rng2);
		r2 = pm3[k2];

		vector<vector<int>> pm4;
		for (auto u = 0; u < pm1.size(); u++) {
			vector<int> t = pm1[u];
			bool b = true;
			for (int j = 0; j < n; j++) {
				if ((t[j] == r0[j]) || (t[j] == r1[j]) || (t[j] == r2[j])) {
					b = false;
					break;
				}
			}
			if ((r0[0] == t[3]) || (r0[n - 1] == t[n - 4]) || (r1[1] == t[3]) || (r1[n - 2] == t[n - 4]) || (r2[2]==t[3]) || (r2[n-3]==t[n-4])) {
				b = false;
			}
			if (b == true) {
				pm4.push_back(t);
			}
		}

		std::mt19937 rng3;
		rng3.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist_pm4(0, pm4.size() - 1);
		int k3 = dist_pm4(rng3);
		r3 = pm4[k3];


		vector<vector<int>> cur_SQ(n, vector<int>(n));
		vector<int> vm1(n);
		for (auto v = 0; v < n; v++) {
			vm1[v] = -1;
		}

		if (check4(r0, r1, r2,r3) == true) {
			cout << "Variant " << i << endl;
			printvector(r0);
			cout << endl;
			printvector(r1);
			cout << endl;
			printvector(r2);
			cout << endl ;
			printvector(r3);
			cout << endl;
			cur_SQ[0] = r0;
			cur_SQ[1] = r1;
			cur_SQ[2] = r2;
			cur_SQ[3] = r3;
			for (auto v = 0; v < 6; v++) {
				cur_SQ[4 + v] = vm1;
			}
			SQmasks.push_back(cur_SQ);
			//print_sq(cur_SQ);
		}
		

	}

}



void bittwidling_count3_10() {
	int n = 10;
	vector<vector<int>> pm1;
	generate_permutations(n, pm1, false);

	vector<uint64_t> d1(pm1.size());
	vector<uint64_t> d2_2(pm1.size());
	vector<uint64_t> d2_3(pm1.size());

	for (auto i = 0; i < pm1.size(); i++) {
	//	printvector(pm1[i]);
		//cout << endl;
		for (uint64_t j = 0; j < 6; j++) {
		//	cout << "current shift @ " << j*n + pm1[i][j] << endl;
			d1[i] |= (uint64_t )1 << (j*n + pm1[i][j]);
		//	bitset<64> x(d1[i]);
		//	cout << x << endl;
		}
		for (uint64_t j = 6; j <10; j++) {
			//	cout << "current shift @ " << j*n + pm1[i][j] << endl;
			d2_2[i] |= (uint64_t)1 << ((j-6)*n + pm1[i][j]);
			d2_3[i] |= (uint64_t)1 << ((j - 6)*n + pm1[i][j]);
			//	bitset<64> x(d1[i]);
			//	cout << x << endl;
		}
		//diagonal elements
		d2_2[i] |= (uint64_t)1 << (4*n+pm1[i][1]);
		d2_2[i] |= (uint64_t)1 << (5*n + pm1[i][n-2]);

		d2_3[i] |= (uint64_t)1 << (4 * n + pm1[i][2]);
		d2_3[i] |= (uint64_t)1 << (5 * n + pm1[i][n - 3]);

		//bitset<64> x1(d1[i]);
		//bitset<64> x2(d2[i]);
		//	cout << x1 <<endl<< x2<< endl;
	}

	uint64_t r0_1=0;
	uint64_t r0_2=0;
	for (uint64_t j = 0; j < 6; j++) {
		//	cout << "current shift @ " << j*n + pm1[i][j] << endl;
		r0_1 |= (uint64_t)1 << (j*n + pm1[0][j]);
		//	bitset<64> x(d1[i]);
		//	cout << x << endl;
	}
	for (uint64_t j = 6; j <10; j++) {
		//	cout << "current shift @ " << j*n + pm1[i][j] << endl;
		r0_2 |= (uint64_t)1 << ((j - 6)*n + pm1[0][j]);		
		//	bitset<64> x(d1[i]);
		//	cout << x << endl;
	}
	r0_2 |= (uint64_t)1 << (4 * n + pm1[0][0]);
	r0_2 |= (uint64_t)1 << (5 * n + pm1[0][n -1]);
	bitset<64> x1(r0_1);
	bitset<64> x2(r0_2);
	cout << x1 <<endl<< x2<< endl;


	//counting 3 rows
	double t0 = cpuTime();
	uint64_t k = 0;
	for (int i = 0; i < pm1.size(); i++) {
		if ((d1[i] & r0_1) == 0) {
			if ((d2_2[i] & r0_2) == 0) {

				uint64_t t1 = d1[i] | r0_1;
				uint64_t t2 = d2_2[i] | r0_2;				

				for (int j = 0; j < pm1.size(); j++) {
					if ((d1[j] & t1) == 0) {
						if ((d2_3[j] & t2) == 0) {
							k++;
							if (k % 100000000 == 0) {
								cout << k << endl;
							}
						}
					}
				}
			}
		}
	}
	double t1 = cpuTime();

	cout << "k = " << k << ". time spent = " << t1 - t0 << " seconds\n";



}

void tv10_to_int(vector<int> &tv, int row_num, bool diag, uint64_t &p1, uint64_t &p2) {
	p1 = 0;
	p2 = 0;
	for (uint64_t j = 0; j < 6; j++) {
		p1 |= (uint64_t)1 << (j * 10 + tv[j]);
	}
	for (uint64_t j = 6; j <10; j++) {
		p2 |= (uint64_t)1 << ((j - 6) * 10 + tv[j]);
	}
	if (diag == true) {
		p2 |= (uint64_t)1 << (4 * 10 + tv[row_num]);
		p2 |= (uint64_t)1 << (5 * 10 + tv[10 - row_num - 1]);
	}
}

void tv10_to_int_4_6(vector<int> &tv, uint64_t &p1, uint64_t &p2) {
	p1 = 0;
	p2 = 0;
	for (uint64_t j = 0; j < 4; j++) {
		p1 |= (uint64_t)1 << (j * 10 + tv[j]);
	}
	for (uint64_t j = 4; j <10; j++) {
		p2 |= (uint64_t)1 << ((j - 4) * 10 + tv[j]);
	}
}


void bittwidling_count3_10_rc1() {
	int n = 10;
	vector<vector<int>> pm1;
	generate_permutations(n, pm1, false);
	cout << pm1.size() << " possible permutations\n";
	uint64_t r0_1 = 0;
	uint64_t r0_2 = 0;
	tv10_to_int(pm1[0], 0, false, r0_1, r0_2);

	vector<uint64_t> t1(pm1.size());
	vector<uint64_t> t2(pm1.size());
	for (auto i = 0; i < pm1.size(); i++) {
		tv10_to_int(pm1[i], 0, false, t1[i], t2[i]);
	}

	cout << "Filtering permutations that intersect with the first one\n";
	vector<bool> ind(pm1.size());

	for (int i = 0; i < pm1.size(); i++) {
		ind[i] = ((r0_1&t1[i]) == 0) && ((r0_2&t2[i]) == 0);
	}

	vector<vector<int>> pmf;
	for (int i = 0; i < pm1.size(); i++) {
		if (ind[i] == true) {
			pmf.push_back(pm1[i]);
		}
	}
	cout << "size of filtered vector is " << pmf.size() << endl;

	tv10_to_int(pm1[0], 0, true, r0_1, r0_2);

	vector<uint64_t> d1(pmf.size());

	vector<uint64_t> d2_2(pmf.size());
	vector<uint64_t> d2_3(pmf.size());
	for (auto i = 0; i < pmf.size(); i++) {
		tv10_to_int(pmf[i], 1, true, d1[i], d2_2[i]);
		uint64_t tmp;
		tv10_to_int(pmf[i], 2, true, tmp, d2_3[i]);
	}

	vector<unsigned int> ind2(pmf.size());
	uint64_t cnt2 = 0;
	for (auto i = 0; i < pmf.size(); i++) {
		bool t = ((r0_1&d1[i]) == 0) & ((r0_2&d2_2[i]) == 0);
		if (t == true) {
			ind2[cnt2] = i;
			cnt2++;
		}
	}
	double dt3_s = cpuTime();
	cout << "Number of variants for two rows is " << cnt2 << "\n";
	uint64_t cnt3 = 0;
	for (auto i = 0; i < cnt2; i++) {
		uint64_t u_1 = r0_1 | d1[ind2[i]];
		uint64_t u_2 = r0_2 | d2_2[ind2[i]];
		for (auto j = 0; j < pmf.size(); j++) {
			bool t = ((u_1 & d1[j]) == 0)& ((u_2 & d2_3[j]) == 0);
			if (t == true) {
				cnt3++;
				if (cnt3 % 100000000 == 0) { cout << cnt3 << endl; }
			}
		}
	}
	double dt3_e = cpuTime();
	cout << "Total number of 3 rows is " << cnt3 << ". time spent = " << dt3_e - dt3_s << " seconds\n";

}



void count_fill4(vector<vector<int>> r4) {
	vector<vector<int>> pm;
	generate_permutations(10, pm, true);
	vector<uint64_t> p1(pm.size());
	vector<uint64_t> p2(pm.size());
	for (auto i = 0; i < pm.size(); i++) {
		tv10_to_int_4_6(pm[i], p1[i], p2[i]);
	}
	vector<uint64_t> tvs(10);
	for (auto i = 0; i < 4; i++) {
		for (auto j = 0; j < 10; j++) {
			tvs[r4[i][j]] |= 1ULL << ((i * 10) + j);
		}
	}

	vector<uint64_t> wv;
	vector<int> inds(11);
	for (auto j = 0; j<10; j++) {
		for (auto i = 0; i < p1.size(); i++) {
			if (p1[i] == tvs[j]) {
				wv.push_back(p2[i]);
			}
		}
		inds[j + 1] = wv.size();
	}

	//debug
	for (int i = 0; i < inds.size(); i++) {
		cout << inds[i] << " ";
	}
	cout << endl;

	uint64_t count = 0;
	int d = 0;
	vector<int> c_ind(10);
	uint64_t c_s = 0;
	// why dont i write 10-fold for ? rly
	// as long as there is no recursion we are fine.

	for (auto it1 = inds[0]; it1 < inds[1]; it1++) {
		if ((c_s&wv[it1]) == 0) {
			c_s |= wv[it1];
			for (auto it2 = inds[1]; it2 < inds[2]; it2++) {
				if ((c_s&wv[it2]) == 0) {
					c_s |= wv[it2];
					for (auto it3 = inds[2]; it3 < inds[3]; it3++) {
						if ((c_s&wv[it3]) == 0) {
							c_s |= wv[it3];
							for (auto it4 = inds[3]; it4 < inds[4]; it4++) {
								if ((c_s&wv[it4]) == 0) {
									c_s |= wv[it4];
									for (auto it5 = inds[4]; it5 < inds[5]; it5++) {
										if ((c_s&wv[it5]) == 0) {
											c_s |= wv[it5];
											for (auto it6 = inds[5]; it6 < inds[6]; it6++) {
												if ((c_s&wv[it6]) == 0) {
													c_s |= wv[it6];
													for (auto it7 = inds[6]; it7 < inds[7]; it7++) {
														if ((c_s&wv[it7]) == 0) {
															c_s |= wv[it7];
															for (auto it8 = inds[7]; it8 < inds[8]; it8++) {
																if ((c_s&wv[it8]) == 0) {
																	c_s |= wv[it8];
																	for (auto it9 = inds[8]; it9 < inds[9]; it9++) {
																		if ((c_s&wv[it9]) == 0) {
																			c_s |= wv[it9];
																			for (auto it10 = inds[9]; it10 < inds[10]; it10++) {
																				if ((c_s&wv[it10]) == 0) {
																					count++;
																					if (count % 1000000 == 0) {
																						cout << count << endl;
																					}
																				}
																			}
																			c_s ^= wv[it9];
																		}
																	}
																	c_s ^= wv[it8];
																}
															}
															c_s ^= wv[it7];
														}
													}
													c_s ^= wv[it6];
												}
											}
											c_s ^= wv[it5];
										}
									}
									c_s ^= wv[it4];
								}
							}
							c_s ^= wv[it3];
						}
					}
					c_s ^= wv[it2];
				}
			}
			c_s ^= wv[it1];
		}

	}

	cout << "Count = " << count << endl;

}



void bittwidling_count4_10_rnd(int k) {
	int n = 10;
	vector<vector<int>> pm1;
	generate_permutations(n, pm1, false);
	cout << pm1.size() << " possible permutations\n";
	uint64_t r0_1 = 0;
	uint64_t r0_2 = 0;
	tv10_to_int(pm1[0], 0, false, r0_1, r0_2);

	vector<uint64_t> t1(pm1.size());
	vector<uint64_t> t2(pm1.size());
	for (auto i = 0; i < pm1.size(); i++) {
		tv10_to_int(pm1[i], 0, false, t1[i], t2[i]);
	}

	cout << "Filtering permutations that intersect with the first one\n";
	vector<bool> ind(pm1.size());

	for (int i = 0; i < pm1.size(); i++) {
		ind[i] = ((r0_1&t1[i]) == 0) && ((r0_2&t2[i]) == 0);
	}

	vector<vector<int>> pmf;
	for (int i = 0; i < pm1.size(); i++) {
		if (ind[i] == true) {
			pmf.push_back(pm1[i]);
		}
	}
	cout << "size of filtered vector is " << pmf.size() << endl;

	tv10_to_int(pm1[0], 0, true, r0_1, r0_2);

	vector<uint64_t> d1(pmf.size());

	vector<uint64_t> d2_2(pmf.size());
	vector<uint64_t> d2_3(pmf.size());
	vector<uint64_t> d2_4(pmf.size());
	for (auto i = 0; i < pmf.size(); i++) {
		tv10_to_int(pmf[i], 1, true, d1[i], d2_2[i]);
		uint64_t tmp;
		tv10_to_int(pmf[i], 2, true, tmp, d2_3[i]);
		tv10_to_int(pmf[i], 3, true, tmp, d2_4[i]);
	}

	vector<unsigned int> ind2(pmf.size());
	uint64_t cnt2 = 0;
	for (auto i = 0; i < pmf.size(); i++) {
		bool t = ((r0_1&d1[i]) == 0) & ((r0_2&d2_2[i]) == 0);
		if (t == true) {
			ind2[cnt2] = i;
			cnt2++;
		}
	}
	double dt3_s = cpuTime();
	cout << "Number of variants for two rows is " << cnt2 << "\n";

	uint64_t cnt3 = 0;

	std::mt19937 rng1;
	rng1.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type>r2 (0, cnt2 - 1);

	vector<int> res(k);
	vector<int> size_k1(k);
	vector<int> size_k2(k);
	vector<int> v_k1(k);
	vector<int> v_k2(k);	

	for (auto i = 0; i < k; i++) {		
		int k1 = r2(rng1);
		v_k1[i] = k1;
		//randomly selected 2nd row
		vector<int> ind3;
		uint64_t u_1 = r0_1 | d1[ind2[k1]];
		uint64_t u_2 = r0_2 | d2_2[ind2[k1]];
		
		for (auto j = 0; j < pmf.size(); j++) {
			bool t = ((u_1 & d1[j]) == 0)& ((u_2 & d2_3[j]) == 0);
			if (t == true) {
				ind3.push_back(j);								
			}
		}
		std::mt19937 rng2;
		rng2.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type>r3(0, ind3.size()- 1);
		size_k1[i] = ind3.size();

		int k2 = r3(rng2);
		v_k2[i] = k2;
		//randomly selected 3rd row
		vector<int> ind4;
		u_1 |= d1[ind3[k2]];
		u_1 |= d2_3[ind3[k2]];

		for (auto j = 0; j < pmf.size(); j++) {
			bool t = ((u_1 & d1[j]) == 0)& ((u_2 & d2_4[j]) == 0);
			if (t == true) {
				ind4.push_back(j);
			}
		}		
		res[i] = ind4.size();
		if (i % 1000 == 0) {
			cout << i << endl;
		}
	}
	//check for collisions;
	for (auto i = 0; i < v_k1.size(); i++) {
		for (auto j = i + 1; j < v_k1.size(); j++) {
			if (v_k1[i] == v_k1[j]) {
				if (v_k2[i] == v_k2[j]) {
					cout << "Collision\n";
				}
			}
		}
	}
	uint64_t tsum_sz3 = 0;
	for (auto i = 0; i < size_k1.size(); i++) {
		tsum_sz3 += size_k1[i];
	}


	uint64_t tsum = 0;
	for (auto i = 0; i < res.size(); i++) {
		tsum += res[i];
	}
	double avg = ((double)tsum) / k;
	double avg_sz3 = ((double)tsum_sz3) / k;
	
	double dt3_e = cpuTime();
	ofstream out;
	out.open("D:\\LSTests\\counting_3_4.log");


	cout << "Averaged for " << k << endl;
	cout << "Average size of pool for 3 " << avg_sz3 << endl;
	cout<<"Average number of 4 fills " << avg << ". time spent = " << dt3_e - dt3_s << " seconds\n";

	out << "Averaged for " << k << endl;
	out << "Average size of pool for 3 " << avg_sz3 << endl;
	out << "Average number of 4 fills" << avg << ". time spent = " << dt3_e - dt3_s << " seconds\n";
	out.close();
}


vector<uint64_t> estimate4_sample(int n, int k) {
	vector<uint64_t> res(k);
	vector<vector<vector<int>>> SQM;
	generate_4r_rnd_rc1(n, k, SQM);

	unsigned long long lim = 1000000000000000;
	vector<vector<vector<int>>> SQ;
	for (auto i = 0; i < SQM.size(); i++) {		
		string outname = "D:\\LSTests\\1\\r4_s_" + inttostr(i) + ".log";
		res[i]=Generate_DLS_masked_nocheck(10, true, lim, true, false, outname, SQM[i], SQ);
	}

	uint64_t tsum = 0;
	for (auto i = 0; i < res.size(); i++) {
		tsum += res[i];
	}
	uint64_t avg = tsum / k;
	cout << "Average for " << k << " is " << avg << endl;
	ofstream out;
	out.open("D:\\LSTests\\1\\report_4s.log");
	for (auto i = 0; i < k; i++) {
		out << res[i] << endl;
	}
	out << "Average for " << k << " is " << avg << endl;		
	out.close();
	return res;
}

void Test_rnd_rc4(int n, int n_of_squares) {
	vector<vector<vector<int>>> SQM;
	generate_4r_rnd_rc1(n, 1, SQM);
	vector<vector<int>> SQMask = SQM[0];
	string logfilename = "D:\\LSTests\\";
	for (auto i = 0; i < n; i++) {
		logfilename += inttostr(SQMask[1][i]);
	}
	logfilename += "rc4.log";
	ofstream out;
	out.open(logfilename);
	out << "LS Mask:\n";
	for (auto i = 0; i < n; i++) {
		for (auto j = 0; j < n; j++) {
			out << SQMask[i][j] << " ";
		}
		out << endl;
	}
	out.close();
	Generate_DLS_masked_compare(n, true, n_of_squares, logfilename, SQMask);
}


void main() {
	//bittwidling_count4_10_rnd(100000);
	//estimate4_sample(10, 1000);

//vector<vector<vector<int>>> SQM;
//	generate_4r_rnd_rc1(10, 10, SQM);


	vector<vector<int>> ax{
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
		{ 2, 3, 4, 9, 8, 1, 0, 5, 6, 7 },
		{ 3, 4, 9, 8, 2, 7, 1, 0, 5, 6 },
		{ 8, 7, 6, 5, 0, 9, 4, 3, 2, 1 }
	};
	vector<vector<int>> bx{
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
		{ 1, 2, 0, 4, 3, 7, 9, 8, 5, 6 },
		{ 7, 3, 5, 9, 0, 4, 8, 6, 2, 1 },
		{ 3, 5, 6, 8, 9, 0, 4, 1, 7, 2 }
	};
	//	count_fill4_rc1(a);
	double d1 = cpuTime();
//	count_fill4(ax);
//	bittwidling_count3_10_rc1();
//	double d2 = cpuTime();
//	cout << "It took " << d2 - d1 << " seconds\n";
//	bittwidling_count3_10();
	//tv_find_sat_enc(10, true);
	//new_tv_encoding(8);
	//new_tv_encoding(9);
	//new_tv_encoding(10);
	vector<vector<int>> m_diag{
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ -1,-1, 1,-1,-1,-1,-1, 1,-1,-1 },
		{ -1,-1,-1, 1,-1,-1, 1,-1,-1,-1 },
		{ -1,-1,-1,-1, 1, 1,-1,-1,-1,-1 },
		{ -1,-1,-1,-1, 1, 1,-1,-1,-1,-1 },
		{ -1,-1,-1, 1,-1,-1, 1,-1,-1,-1 },
		{ -1,-1, 1,-1,-1,-1,-1, 1,-1,-1 },
		{ -1, 1,-1,-1,-1,-1,-1,-1, 1,-1 },
		{ 1,-1,-1,-1,-1,-1,-1,-1,-1, 1 },
	};

	vector<vector<int>> a{
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
		{ 2, 3, 4, 9, 8, 1, 0, 5, 6, 7 },
		{ 3, 4, 9, 8, 2, 7, 1, 0, 5, 6 },
		{ 8, 7, 6, 5, 0, 9, 4, 3, 2, 1 },
		{ 5, 0, 1, 7, 6, 3, 2, 8, 9, 4 },
		{ 6, 5, 0, 1, 7, 2, 8, 9, 4, 3 },
		{ 4, 9, 8, 2, 3, 6, 7, 1, 0, 5 },
		{ 7, 6, 5, 0, 1, 8, 9, 4, 3, 2 },
		{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
		{ 1, 2, 3, 4, 9, 0, 5, 6, 7, 8 }
	};
	vector<vector<int>> b{
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
		{ 1, 2, 0, 4, 3, 7, 9, 8, 5, 6 },
		{ 7, 3, 5, 9, 0, 4, 8, 6, 2, 1 },
		{ 3, 5, 6, 8, 9, 0, 4, 1, 7, 2 },
		{ 4, 9, 7, 2, 6, 8, 1, 5, 0, 3 },
		{ 5, 8, 4, 6, 7, 1, 3, 2, 9, 0 },
		{ 8, 4, 9, 1, 2, 3, 7, 0, 6, 5 },
		{ 6, 7, 3, 0, 1, 2, 5, 9, 4, 8 },
		{ 9, 0, 1, 5, 8, 6, 2, 4, 3, 7 },
		{ 2, 6, 8, 7, 5, 9, 0, 3, 1, 4 }
	};

	estimate4_sample(10, 10000);

	vector<vector<int>> pm;
	vector<vector<int>> pm_diag;
	generate_permutations(10, pm, false);
	generate_permutations(10, pm_diag, true);

	
	vector<vector<int>> tvr_bel_diag = getTrans_mod(a, true);

	vector<vector<int>> tvr_bel = getTrans_mod(a, false);


	cout << "Total transversals of order 10 " << pm.size() << endl;
	cout << "Total diagonal transversals of order 10 " << pm_diag.size() << endl;
	cout << "Total transversals of Brown square" << tvr_bel.size() << endl;
	cout << "Total diagonal transversals of Brown square" << tvr_bel_diag.size() << endl;

	//generate_3r_rnd_rc1(10, 20);
	

	vector<vector<int>> ls5(5, vector<int>(5));
	for (int i = 0; i < 5; i++) {
		ls5[0][i] = i;
	}
	for (int i = 1; i < 5; i++) {
		for (int j = 0; j < 5; j++)
		{
			ls5[i][j] = -1;
		}
	}
	vector<vector<int>> ls6(6, vector<int>(6));
	for (int i = 0; i < 6; i++) {
		ls6[0][i] = i;
	}
	for (int i = 1; i < 6; i++) {
		for (int j = 0; j < 6; j++)
		{
			ls6[i][j] = -1;
		}
	}

	vector<vector<int>> ls7(7, vector<int>(7));
	for (int i = 0; i < 7; i++) {
		ls7[0][i] = i;
	}
	for (int i = 1; i < 7; i++) {
		for (int j = 0; j < 7; j++)
		{
			ls7[i][j] = -1;
		}
	}

	vector<vector<int>> ls8(8, vector<int>(8));
	for (int i = 0; i < 8; i++) {
		ls8[0][i] = i;
	}
	for (int i = 1; i < 8; i++) {
		for (int j = 0; j < 8; j++)
		{
			ls8[i][j] = -1;
		}
	}

	vector<vector<int>> a_d = compute_masked_LS(a, m_diag);
	vector<vector<int>> a_5 = compute_masked_LS(a, 50);
	vector<vector<int>> b_4 = compute_masked_LS(b, 40);
	vector<vector<int>> b_45 = compute_masked_LS(b, 45);
	vector<vector<int>> a_4 = compute_masked_LS(a, 40);
	vector<vector<int>> b_5 = compute_masked_LS(b, 50);
	unsigned long long lim = 1000000000000000;
	Generate_DLS_masked_DLXrefresh(10, true, lim, "D:\\LSTests\\b45.log", b_45);

	Test_rnd_rc4(10, 100000);
	//Generate_DLS_masked_crosscheck(10, true, 100000, "D:\\LSTests\\test_13_07.log", a_4);
	//Generate_DLS_masked_compare(10, true, 100000, "D:\\LSTests\\test_13_07.log", a_4);

	//generate_permutations_masked_rc1(10, pm2, a40, true);

	//vector<vector<int>> a_d = compute_masked_LS(a, m_diag);
	vector<vector<int>> b_d = compute_masked_LS(b, m_diag);
	vector<vector<vector<int>>> SQ;
	
	
	
	//Generate_DLS_masked_nocheck(10, true, lim, true, false, "D:\\LSTests\\r3_8.log", r3_8, SQ);
	
	
	cout << endl << "Finish";
	int u;
	cin >> u;


}


