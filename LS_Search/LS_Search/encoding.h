#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <Windows.h>
#include <chrono> 
#include <iomanip>
using namespace std;

enum class AMO_Type { Pairwise, Binary, Commander, Product, Sequential, Bimander };
enum class Encoding_Type { Naive, Minimal, Extended };


class C_Cell {
public:
	vector<int> variables;
	int value;
	::C_Cell(vector<int> var) {
		variables = var;
	}
};

class B_Cell {// Binary cell
public:
	vector<int> variables;
	int value;	
	int maxvalue; // \leq kind
	::B_Cell(int mv, vector<int> var) {
		maxvalue = mv;
		variables = var;
	}
};
void ineq(B_Cell A, B_Cell B, int &nv, vector<vector<int>>& E);

int greater(B_Cell a, B_Cell b, int & nv_var, vector<vector<int>> & E);

void ALO(B_Cell t, vector<vector<int>> &E);

void ALO(vector<int>t, vector<vector<int>> &E);

void ALO(C_Cell t, vector<vector<int>> & E);

void ALO(vector<C_Cell> t, vector<vector<int>> & E);

void AMO_pairwise(vector<int>t, vector<vector<int>> &E);

vector<int> bin_pp(vector<int> c);

vector<vector<int>> Bin_Rep(int d);

void AMO_Binary(vector<int>t, int & newvar, vector<vector<int>> &E);

void AMO_Commander(vector<int> t, int m, int & newvar, vector<vector<int>> &E);

void AMO_Product(vector<int> t, int p, int q, int & newvar, vector<vector<int>> &E);

void AMO_Seq(vector<int> t, int & newvar, vector<vector<int>>& E);

int logm(int m);

void AMO_Bimander(vector<int> t, int m, int & newvar, vector<vector<int>> & E);



void AMO(vector<int> t, AMO_Type amo, int p, int q, int m, int & newvar, vector<vector<int>> & E);

void AMO(C_Cell t, AMO_Type amo, int p, int q, int m, int & newvar, vector<vector<int>> & E);

void AMO(vector<C_Cell> t, AMO_Type amo, int p, int q, int m, int & newvar, vector<vector<int>> & E);

void OnlyOne(vector<int> t, vector<vector<int>> & E);

void OnlyOne(C_Cell t, vector<vector<int>> & E);

void OnlyOne(vector<C_Cell> t, vector<vector<int>> & E);

void var_eq_and(int leftpart, vector<int> rightpart, vector<vector<int>> & E);

void var_eq_or(int leftpart, vector<int> rightpart, vector<vector<int>> & E);

void var_eq_and(int lp, int rp1, int rp2, vector<vector<int>> & E);

void var_eq_or(int lp, int rp1, int rp2, vector<vector<int>> & E);

void var_eq_eq(int lp, int rp1, int rp2, vector<vector<int>> & E);

void var_eq_if(int lp, int cond, int rp1, int rp2, int &nv_var, vector<vector<int>>&E);

void var_eq_if(B_Cell a, int cond, B_Cell rp1, B_Cell rp2, int &nv_var, vector<vector<int>>&E);

int greater(B_Cell a, B_Cell b, int & nv_var, vector<vector<int>> & E);

void encode_comparator(B_Cell in_a, B_Cell in_b, B_Cell out_a, B_Cell out_b, int &nv_var, vector<vector<int>>&E);