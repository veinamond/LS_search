#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <algorithm>
#include <sstream>
#include "minisat22_wrapper.h"
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
					break;
				}
			}
		}
		if (a == true) {
			TVSET_new.push_back(TVSET[i]);
		}
	}
	TVSET = TVSET_new;
	cout << "Size after filtering " << TVSET_new.size() << endl;
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

	if (tmp[v]==1) {
		//продолжаем только в этом случае		
		if (r->firstchild != NULL) {
			TV_check_TRT_mod(r->firstchild, TVSet_SQUARE,ind, indices, tmp);
		}
		else {
			indices.push_back(r->index);
		}
	}
	tmp[v]--;
	if (r->next_sibling != NULL) {
		r = r->next_sibling;
		TV_check_TRT_mod(r, TVSet_SQUARE, ind, indices,  tmp);
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
	if (LSmask[r->level][r->value]==-1){
		if (r->firstchild != NULL) {
			TV_check_TRT_masked(r->firstchild, LSmask, indices, tmp);
		}
		else {
			indices.push_back(r->index);
		}
	}
	else{
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
	while (i!= c) {		
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
	while (j!= &h) {
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

void search_limited(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr, bool &cont, int &limit, bool &count_only, int &count) {
	//pd = partial solution
	if (k > 10) {
		cout << "we are in trouble" << endl;

	}
	//	cout << "Search " << k << endl;
	if (cont==true){
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
			if (count % 1000000 == 0) { cout << count << endl; }
			//print_solution(ps);
		}
		else {
			DLX_column * c = NULL;
			choose_c(h, c);
			//cout << "picked column " << c->column_number << endl;
			cover(c);
			DLX_column * r = c->Down;
			while ((r != c)&&(cont==true)) {
				ps.push_back(r);
				DLX_column * j;
				j = r->Right;
				while (j != r) {
					cover(j->Column);
					j = j->Right;
				}

				search_limited(k + 1, h, ps, tvr, cont,limit,count_only,count);
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
		DLX_column * r=c->Down;
		while (r!= c) {			
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
		ct->Left= lastleft;
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
			ctve->Down= columns[k];
			ctve->Up = columns[k]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i;			
		//	ctve->column_number = k;
			ctve->size=-10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);
		}
		
		for (int j = 0; j < tvrow.size()-1; j++) {
			tvrow[j]->Right = tvrow[j + 1];
			tvrow[j]->Right->Left= tvrow[j];
		}
		tvrow[tvrow.size()-1]->Right = tvrow[0];
		tvrow[0]->Left = tvrow[tvrow.size()-1];
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

	while ((r==false)&&(j < dim)) {
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
			if ((i == 10) && (diag == true)){
				int md = 0;
				int ad = 0;
				for (int l = 0; l < ct.size(); l++) {
					if (ct[l] == l) {md++;}
					if (ct[l] == ct.size()-l-1) {ad++;}					
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
		while (nextTrans_ext(a, h, co, x,diag) == true) {
			res[num]=h;
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
	while ((k >= 0)&&(found==false)) {
		if (dim < 3) {
			for (int j = k-4; j <= k + 4; j++) {
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
				k = k + dim-r;				
			}
			else {
				k = k - dim+r;
			}
		}
	}
	return res;
}
	
vector<int> getTrans_mod_ind(vector<vector<int>> &a,vector<vector<int>> &TVSET, bool diag) {	
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
			res[num] = indexof(TVSET,h);
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

vector<int> getTrans_mod_ind_tv(vector<int> &tvind, vector<vector<int>> &TVSET,  vector<vector<int>> &TVSET_SQUARE, bool diag) {
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
		if (i==9)
		{
			x[a[i][h[i]]] = false;
			j++;
			goto label2;
		}
		i++;
		h[i]=0;
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
	vector<vector<int>> b(10,vector<int>(10));
	int c;

	hb[0] = 0;
	c = 0;
	for (k = 1; k <= 10; k++) {
		hb[k] = hb[k - 1] + ltr[k - 1];
	}
	i= 0; 
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
void read_pairs_from_file(string filename, vector<vector<vector<int>>> &Squares) {
	ifstream in;
	in.open(filename);
	vector<int> a;
	string s;
	while (in.good()) {
		getline(in, s);
		if (s.length()>2){
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
				tmp1.push_back(a[t*200+i*20+j]);
				
				tmp2.push_back(a[t * 200 + i * 20 +10 + j]);				
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


void check_belyaev(vector<vector<int>> SQ,bool diag, string filename) {
	ofstream out;
	
	vector<vector<int>> tr(10000, vector<int>(10));
	vector<int> ltr(10);
	vector<int> hb(11);
	vector<vector<vector<int>>> orts(10000);
	getTrans(SQ, diag,tr,ltr,hb);
	int sum = 0;
	for (int i = 0; i < ltr.size(); i++) {
		sum += ltr[i];
	}
	
	vector<vector<int>> trv;
	int t = getOrt(trv,ltr,tr,orts);
	if (trv.size()!=0){
		out.open(filename, ios::app);
		out << "Square " << endl;
		//vector<vector<int>> SQ(10, vector<int>(10));
		//construct_square_from_tv(TVset_SQUARE, TVind, SQ);
		for (int i = 0; i < SQ.size(); i++) {
			for (int j = 0; j < SQ.size(); j++) {
				out << SQ[i][j] << " ";
			}
			out << endl;
		}
		out << "Bel: Total: " << sum << " transversals" << endl;
		out << "Found " << trv.size() << "sets of disjoint transversals" << endl;
		if (trv.size()>0){
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
	search(0,*root, ps,tvr);
	for (int i = 0; i < tvr.size(); i++) {
		sort(tvr[i].begin(), tvr[i].end());
	}
	
	for (int i = 0; i < elements.size(); i++) {
		delete elements[i];
	}
	
	if (tvr.size()>0) {
		out << "Square " << endl;
		//vector<vector<int>> SQ(10, vector<int>(10));
		//construct_square_from_tv(TVset_SQUARE, TVind, SQ);
		for (int i = 0; i < SQ.size(); i++) {
			for (int j = 0; j < SQ.size(); j++) {
				out << SQ[i][j] << " ";
			}
			out << endl;
		}
		out << "DLX_old: Total: " << trm.size() << " transversals" << endl;
		out << "Found " << tvr.size() << "sets of disjoint transversals" << endl;		

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

void configure_DLX(DLX_column *&root, vector<DLX_column*>& columns, vector<vector<DLX_column*>> &rows,vector<int> &tvind) {
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
		out << "DLX_new:Total: " << trm.size() << " transversals" << endl;
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

void check_dlx_EXT_LS(int index, TRT *&r,vector<vector<int>> &SQ, vector<vector<int>>& TVSET, DLX_column *&root, vector<DLX_column*>& columns, vector<vector<DLX_column*>>&rows, bool diag, string filename) {


	//vector<int> trm;
	//vector<int> tm(10);
	//TV_check_TRT_mod(r->firstchild, TVset_SQUARE, TVind, trm, tm);

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
		out << "DLX_new_filt:Total: " << trm.size() << " transversals" << endl;
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
	out << "Total time required to process the set is " << t_end - t_start << " seconds" << endl;
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
	out << "Total time required to process the set is " << t_end - t_start << " seconds" << endl;
	out.close();
}
//check_dlx_EXT_LS

void check_squares_DLX_EXT_tv(TRT*& r, vector<vector<int>> & tvinds, vector<vector<int>>& TVSET, vector<vector<int>>& TVSET_SQUARE, DLX_column *&root, vector<DLX_column*>& columns, vector<vector<DLX_column*>>&rows, bool diag, string logfilename) {
	ofstream out;
	double t_start = cpuTime();

	for (int i = 0; i < tvinds.size(); i++) {
		if ((i > 0) && (i % 1000 == 0)) { cout << "Checked " << i << " squares" << endl; }
	//	out.open(logfilename, ios::app);
	//	out << endl << "Square " << i << endl;
	//	out.close();
		check_dlx_EXT(i, r,tvinds[i], TVSET, TVSET_SQUARE, root, columns, rows, diag, logfilename);
	}
	double t_end = cpuTime();
	out.open(logfilename, ios::app);
	out << "Total time required to process the set is " << t_end - t_start << " seconds" << endl;
	out.close();
}

void check_squares_DLX_EXT_LS(TRT*& r, vector<vector<vector<int>>> &SQUARES,  vector<vector<int>>& TVSET, DLX_column *&root, vector<DLX_column*>& columns, vector<vector<DLX_column*>>&rows, bool diag, string logfilename) {
	ofstream out;
	double t_start = cpuTime();

	for (int i = 0; i < SQUARES.size(); i++) {
		if ((i > 0) && (i % 1000 == 0)) { cout << "Checked " << i << " squares" << endl; }
		//	out.open(logfilename, ios::app);
		//	out << endl << "Square " << i << endl;
		//	out.close();
		//tvinds[i], TVSET, TVSET_SQUARE, root, columns, rows, diag, logfilename);
		check_dlx_EXT_LS(i, r, SQUARES[i], TVSET, root, columns, rows, diag, logfilename);
	}
	double t_end = cpuTime();
	out.open(logfilename, ios::app);
	out << "Total time required to process the set is " << t_end - t_start << " seconds" << endl;
	out.close();
}


void free_dlx(vector<DLX_column*> &elements) {
	for (int i = 0; i < elements.size(); i++) {
		elements[i]->Column = NULL;
		elements[i]->Up= NULL;
		elements[i]->Down= NULL;
		elements[i]->Left= NULL;
		elements[i]->Right= NULL;		
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
		for (int j = 0; j < n; j++){			
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
		} while (std::next_permutation(seed_i.begin()+k, seed_i.end()));

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
				bs[seed[0]] = perm.size()-1;
			}
		}
	} while (std::next_permutation(seed.begin(), seed.end()));
	bs.push_back(perm.size() - 1);
//	cout << "Generated " << perm.size() << "permutations" << endl;
}
void SEARCH_TO_DLX(DLX_column &root, vector<vector<char>> & tvset, vector<DLX_column*> & elements) {
	int dimension = tvset[0].size()/2;
	root.Up = NULL;
	root.Down = NULL;
	root.Column = NULL;
	root.row_id = -1;
	root.size = -1;
	//	root.column_number= -1;
	elements.push_back(&root);
	vector<DLX_column *> columns;
	DLX_column * lastleft = &root;
	for (int i = 0; i < 3*dimension* dimension; i++) {
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
			int k =2 * dimension*dimension + curtv[j]*dimension + curtv[dimension+j];
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





void Generate_DLS_masked(int n, bool diag, int limit, string logname, vector<vector<int>>mask) {
	vector<vector<int>> perm_diag;
	generate_permutations_masked(n, perm_diag, mask, diag);
	sort(perm_diag.begin(), perm_diag.end());

	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);

	int size_big = sizeof(elements) + elements.size()*sizeof(elements[0]);
	cout << "The size of elements vector is " << size_big << endl;

	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;
	double t1 = cpuTime();
	int count = 0;
	bool count_only = true;
	search_limited(0, *root, ps, tvr, cont, limit, count_only, count);
	double t2 = cpuTime();
	
	cout << tvr.size()<< " squares generated in " << t2 - t1 << " seconds" << endl;
	//vector<vector<int>>> SQUARES(10, vector<int>(10));	
	
	vector<vector<int>> pm;	
	generate_permutations(n, pm, diag);
	sort(pm.begin(), pm.end());

	DLX_column *root_pm;
	root_pm= new (DLX_column);
	vector<DLX_column*> columns;
	vector<vector<DLX_column*>> rows;
	TVSET_TO_DLX_EXT(*root_pm, pm, columns, rows);

	TRT *r10;
	r10 = new(TRT);
	construct_TR_tree(pm, r10, diag);	
	
	double sc_t1 = cpuTime();
	vector<vector<vector<int>>> SQUARES (tvr.size(),vector<vector<int>>(n,vector<int>(n)));
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

void Generate_DLS_masked_compare(int n, bool diag, int limit, string logname, vector<vector<int>>mask) {
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
	int count = 0;
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


	vector<vector<int>> pm_filt;
	generate_permutations(n, pm_filt, diag);
	sort(pm_filt.begin(), pm_filt.end());
	filter_TV(pm_filt, mask);

	DLX_column *root_pm_filt;
	root_pm_filt = new (DLX_column);
	vector<DLX_column*> columns_filt;
	vector<vector<DLX_column*>> rows_filt;
	TVSET_TO_DLX_EXT(*root_pm_filt, pm_filt, columns_filt, rows_filt);

	TRT *r10_filt;
	r10_filt= new(TRT);
	construct_TR_tree(pm_filt, r10_filt, diag);

	

	double sc_t1 = cpuTime();
	vector<vector<vector<int>>> SQUARES(tvr.size(), vector<vector<int>>(n, vector<int>(n)));
	construct_squares_from_tv_set(perm_diag, tvr, SQUARES);


	//finding transversals using Belyaev alg
	double BTVsearh_0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		vector<vector<int>> trm = getTrans_mod(SQUARES[i], diag);
	}
	double BTVsearh_1 = cpuTime();
	cout << "finding transversals using Belyaev alg finished \n";
	//finding transversals using TRT tree

	double TRTTVsearh_0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		vector<int> trm;
		vector<int> tmp(10);
		TV_check_TRT(r10->firstchild, SQUARES[i], trm, tmp);
	}
	double TRTTVsearh_1 = cpuTime();
	cout << "finding transversals using TRT tree finished \n";

	double TRTTVsearh_filt_0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		vector<int> trm;
		vector<int> tmp(10);
		TV_check_TRT(r10_filt->firstchild, SQUARES[i], trm, tmp);
	}
	double TRTTVsearh_filt_1 = cpuTime();
	cout << "finding transversals using filtered TRT tree finished \n";


	//checking squares with belyev algorithm
	double Belyaev_check0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_belyaev(SQUARES[i], diag, logname);
	}
	double Belyaev_check1 = cpuTime();
	cout << "checking squares with belyev algorithm finished\n";
	//checking squares with old DLX implementation

	double OLDDLX_check0 = cpuTime();
	for (int i = 0; i < SQUARES.size(); i++) {
		check_dlx(SQUARES[i], diag, logname);
	}
	double OLDDLX_check1 = cpuTime();
	cout << "checking squares with old DLX algorithm finished\n";
	//checking squares with new DLX implementation

	double sc_check_t1 = cpuTime();
	check_squares_DLX_EXT_tv(r10, tvr, pm, perm_diag, root_pm, columns, rows, diag, logname);
	double sc_check_t2 = cpuTime();


	double tdlx_new_filt_0 = cpuTime();
	check_squares_DLX_EXT_LS(r10_filt, SQUARES, pm_filt, root_pm_filt, columns_filt, rows_filt, diag, logname);
	double tdlx_new_filt_1 = cpuTime();

	cout << "Number of squares before filtration " << SQUARES.size() << "\n";
	double sc_cf_t1 = cpuTime();
	vector<vector<int>> SQ;
	vector<vector<vector<int>>> F_sq;
	for (int i = 0; i < SQUARES.size(); i++) {
		bool t = false;
		SQ = construct_cf_limited(SQUARES[i], t);
		if (t == false) {
			F_sq.push_back(SQUARES[i]);
		}		//BACKTRACK
	}
	double sc_cf_t2 = cpuTime();

	cout << "Computing reduced canonical forms took " << sc_cf_t2 - sc_cf_t1 << " seconds,\n";
	cout << "Number of squares after filtration " << F_sq.size() << "\n";


	cout << "RESULTS\n";
	cout << "Finding transversals using Belyaev algorithm took " << BTVsearh_1 - BTVsearh_0 << " seconds\n";
	cout << "Finding transversals using TRT algorithm took " << TRTTVsearh_1 - TRTTVsearh_0 << " seconds\n";
	cout << "Finding transversals using TRT algorithm took " << TRTTVsearh_filt_1 - TRTTVsearh_filt_0 << " seconds\n";
	cout << "Checking all SQUARES using Belyaev algorithm took " << Belyaev_check1 - Belyaev_check0 << " seconds\n";
	cout << "Checking all SQUARES using old DLX algorithm took " << OLDDLX_check1 - OLDDLX_check0 << " seconds\n";
	cout << "Checking all SQUARES using new DLX algorithm took " << sc_check_t2 - sc_check_t1 << " seconds\n";
	cout << "Checking all SQUARES using new DLX algorithm with filters took " << tdlx_new_filt_1 - tdlx_new_filt_0 << " seconds\n";

	cout << "Checking all squares took " << sc_check_t2 - sc_check_t1 << " seconds,\n";
	//check_squares_DLX(SQUARES, true, logname);
}

void Generate_DLS_masked_nocheck(int n, bool diag, int limit, bool count_only, bool verbosity, string logname, vector<vector<int>>mask, vector<vector<vector<int>>> &SQUARES) {
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
	int count = 0;
	search_limited(0, *root, ps, tvr, cont, limit, count_only, count);
	double t2 = cpuTime();

	cout << count << " squares generated in " << t2 - t1 << " seconds" << endl;
	//vector<vector<int>>> SQUARES(10, vector<int>(10));	
	/*
	void construct_squares_from_tv_set(vector<vector<int>>&tv_set, vector<vector<int>> &tv_index_sets, vector<vector<vector<int>>> &SQUARES) {
		for (int i = 0; i < tv_index_sets.size(); i++) {
			construct_square_from_tv(tv_set, tv_index_sets[i], SQUARES[i]);
		}
	}
	*/
	if (count_only == false){
		construct_squares_from_tv_set(perm_diag, tvr, SQUARES);
		if (verbosity == true) {
			ofstream out;
			out.open(logname);
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
}

void Generate_DLS(int dimension, int limit, string logname) {
	int n = dimension;
	vector<vector<int>> perm_diag;	
	generate_permutations(n, perm_diag, true);
	
	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);

	int size_big = sizeof(elements) + elements.size()*sizeof(elements[0]);
	cout << "The size of elements vector is " << size_big << endl;

	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;	
	double t1 = cpuTime();
	int count = 0;
	bool count_only = true;
	search_limited(0, *root, ps, tvr,cont,limit,count_only,count);
	double t2 = cpuTime();
	cout << limit << " squares generated in " << t2 - t1 << " seconds" << endl;
	//vector<vector<int>>> SQUARES(10, vector<int>(10));	
	vector<vector<vector<int>>> SQUARES;
	for (int i = 0; i < tvr.size(); i++) {(dimension, vector<int>(dimension));
		vector<vector<int>> SQ(dimension, vector<int>(dimension));
		SQUARES.push_back(SQ);
	}
	
	construct_squares_from_tv_set(perm_diag, tvr, SQUARES);
	check_squares_DLX(SQUARES, true, logname);

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

void LS_SAT_SEARCH(vector<vector<vector<int>>> &LS, vector<vector<int>> &mask) {
	vector<vector<int>> Encoding;
	int nvars;
	Encoding = loadcnffromfile(nvars, "D:\\LStests\\LS_10_2_pw_naive_pw_naive.cnf");
	minisat22_wrapper s;
	
	for (int i = 0; i < mask.size(); i++) {
		for (int j = 0; j < mask[i].size(); j++) {			
			if (mask[i][j] >= 0) {
				vector<int> t;
				t.push_back(i * 100 + j * 10 + mask[i][j]+1);
				cout << t[0] << " ";
				Encoding.push_back(t);
			}
			
		}
	}
	Minisat::Problem p = s.convert_to_problem(Encoding);

	vector<vector<int>> sat_sols;
	s.minisat_solve_incremental(p, sat_sols);

}

void LS_SAT_SEARCH_test(vector<vector<vector<int>>> &LS, vector<vector<int>> &mask1, vector<vector<int>> &mask2) {
	vector<vector<int>> Encoding;
	int nvars;
	Encoding = loadcnffromfile(nvars, "D:\\LStests\\LS_10_2_pw_naive_pw_naive.cnf");
	minisat22_wrapper s;
	
	for (int i = 0; i < mask1.size(); i++) {
		for (int j = 0; j < mask1[i].size(); j++) {
			vector<int> t;
			if (mask1[i][j] >= 0) {
				t.push_back(i * 100 + j * 10 + mask1[i][j] + 1);
				cout << t[0] << " ";
			}
			Encoding.push_back(t);
		}
	}

	for (int i = 0; i < mask2.size(); i++) {
		for (int j = 0; j < mask2[i].size(); j++) {
			vector<int> t;
			if (mask2[i][j] >= 0) {
				t.push_back(i * 100 + j * 10 + mask2[i][j] + 1);
				cout << t[0] << " ";
			}
			Encoding.push_back(t);
		}
	}
	Minisat::Problem p = s.convert_to_problem(Encoding);
	
	vector<vector<int>> sat_sols;
	s.minisat_solve_incremental(p, sat_sols);

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
			if ((i>0)&&(i % 100 == 0)){
				cout << "Depth "<<depth<<", processed " << i << " out of " << res.size() << " variants, current sum "<<sum <<"\n";
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
			if (i == j) { MD[SQ[i][j]]--;}
			if (j == dimension-i-1) { AD[SQ[i][j]]--; }
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
			if (verbosity==true){
				cout << "SQUARE # " << cnt - 1 << "\n";
				for (int u = 0; u < dimension; u++) {
					for (int v = 0; v < dimension; v++) {
						cout << SQ[u][v] << " ";
					}
					cout << "\n";
				}
			}
			inc_ii_D(ii, i, j, dimension, SQ, ROWS, COLUMNS,AD,MD);

		}
		bool MDE = (i!=j) || ((i == j) && (MD[ii]==0));
		bool ADE = (j!=d-i-1) || ((j == d-i-1) && (AD[ii] == 0));


		if ((ROWS[i][ii] == 0) && (COLUMNS[j][ii] == 0)&& MDE && ADE) {			
			SQ[i][j] = ii;
			ROWS[i][ii]++;
			COLUMNS[j][ii]++;
			if (i == j) { MD[SQ[i][j]]++;}
			if (j == d - i - 1) { AD[SQ[i][j]]++;}

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
	
	vector<vector<int>> cur_PLS(1,vector<int>(dimension));
	vector<int> first_row;
	for (int i = 0; i < dimension; i++) first_row.push_back(i);
	cur_PLS[0] = first_row;

	vector<int> ind(perm.size());
	for (int i = 0; i < perm.size(); i++) { ind[i]=i;}
	
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
			std::cout << "LS and MASK sizes dont match @ "<<i<<"\n";
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
		res[i/n][i%n] = -1;		
	}
	return res;
}

void main() {
	vector<vector<int>> m_diag{
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{-1,-1, 1,-1,-1,-1,-1, 1,-1,-1 },
		{-1,-1,-1, 1,-1,-1, 1,-1,-1,-1 },
		{-1,-1,-1,-1, 1, 1,-1,-1,-1,-1 },
		{-1,-1,-1,-1, 1, 1,-1,-1,-1,-1 },
		{-1,-1,-1, 1,-1,-1, 1,-1,-1,-1 },
		{-1,-1, 1,-1,-1,-1,-1, 1,-1,-1 },
		{-1, 1,-1,-1,-1,-1,-1,-1, 1,-1 },
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

		//int c1 = count_PLS(10, 1);
	//cout << "Number of variants of first 2 rows is " << c1 << endl;
	
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


	
	//generate_permutations_masked_rc1(10, pm2, a40, true);
	vector<vector<int>> a50 = compute_masked_LS(a, 50);
	vector<vector<int>> b50 = compute_masked_LS(b, 50);
	vector<vector<int>> a45 = compute_masked_LS(a, 45);
	vector<vector<int>> b45 = compute_masked_LS(b, 45);
	vector<vector<int>> a_d = compute_masked_LS(a, m_diag);
	vector<vector<int>> b_d = compute_masked_LS(b, m_diag);
	Generate_DLS_masked_compare(10, true, 50000, "D:\\LSTests\\test.log", a50);

	vector<vector<vector<int>>> SQ;
	int lim = 100000000000;
	Generate_DLS_masked_nocheck(10, true, lim, true, false, "D:\\LSTests\\test.log", a_d, SQ);
	Generate_DLS_masked_nocheck(10, true, lim, true, false, "D:\\LSTests\\test.log", b_d, SQ);

		cout << endl << "Finish";
	int u;
	cin >> u;
		

}


