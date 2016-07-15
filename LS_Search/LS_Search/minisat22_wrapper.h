#ifndef minisat22_wrapper_h
#define minisat22_wrapper_h

#include <errno.h>

#include <signal.h>
#include <iostream>
#include <vector>
#include <signal.h>
#include <fstream>
#include <string>
#include <sstream>

#include "utils/System.h"
#include "utils/ParseUtils.h"
#include "utils/Options.h"
#include "core/Dimacs.h"
#include "core/Solver.h"

using namespace Minisat;

class minisat22_wrapper
{
public:
	
	void readClause(StreamBuffer& in, vec<Lit>& lits);
	void convertClause(std::vector<int>& in, vec<Lit>& lits);
	void parse_DIMACS_to_problem(std::istream& input, Problem& cnf);

	void parse_DIMACS_from_inc( std::vector<int> &cnf_vec, Problem& cnf);
	void printProblem(const Problem& p, std::ostream& out);
	int minisat_solve(Problem &CNF_in, std::vector<int> assumpts, std::vector<int> &sat_solution);
	int minisat_solve_noout(Problem &CNF_in, std::vector<int> assumpts, std::vector<int> &sat_solution);
	
	int minisat_solve_incremental(Problem &CNF_in, std::vector<std::vector<int>> &sat_solutions);
	int minisat_solve_find_tv(Problem &CNF_in, int n, std::vector<int> assumpts, std::vector<std::vector<int>> &sat_solutions);
	


	int minisat_solve_inc(Problem &CNF_in, std::vector<std::vector<int>> assumpts_vectors, std::vector<int> &sat_solution);
	int minisat_solve_file(int argc, char* argv[]);
	Minisat::Problem convert_to_problem(std::vector<std::vector<int>>& M);
};

#endif