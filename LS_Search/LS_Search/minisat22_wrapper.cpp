#include "minisat22_wrapper.h"
#include <iostream>

 void minisat22_wrapper :: readClause(StreamBuffer& in, vec<Lit>& lits) 
{
    int     parsed_lit, var;
    lits.clear();
    for (;;){
        parsed_lit = parseInt(in);
        if (parsed_lit == 0) break;
        var = abs(parsed_lit)-1;
        lits.push( (parsed_lit > 0) ? mkLit(var) : ~mkLit(var) );
    }
}
 void minisat22_wrapper::convertClause(std::vector<int>& in, vec<Lit>& lits)
 {
	 int var;
	 lits.clear();
	 for (int i = 0; i < in.size();i++) {
		 if (in[i]== 0) break;
		 var = abs(in[i]) - 1;
		 lits.push((in[i] > 0) ? mkLit(var) : ~mkLit(var));
	 }
 }

void minisat22_wrapper :: parse_DIMACS_to_problem(std::istream& input, Problem& cnf)
{
	StreamBuffer in(input);
	Disjunct* lits = 0;
    int vars    = 0;
    int clauses = 0;
    int cnt     = 0;
    for (;;){
        skipWhitespace(in);
		if (in.eof()) break;
        else if (*in == 'p'){
            if (eagerMatch(in, "p cnf")){
                vars    = parseInt(in);
                clauses = parseInt(in);
            }else{
                printf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
            }
        } else if (*in == 'c' || *in == 'p')
            skipLine(in);
        else{
            cnt++;
			lits = new Disjunct;
            readClause(in, *lits);
			cnf.push_back(lits);
		}
    }
}

// read cnf from resource file
void minisat22_wrapper :: parse_DIMACS_from_inc( std::vector<int> &cnf_vec, Problem &cnf )
{
	Disjunct *lits = 0;
	int parsed_lit, var;
	bool IsNewClause = true;
	
	for ( unsigned i = 0; i < cnf_vec.size(); i++ ) {
		parsed_lit = cnf_vec[i];
		if ( parsed_lit == 0 ) {
			cnf.push_back(lits);
			IsNewClause = true;
			continue;
		}
		var = abs(parsed_lit)-1;
		if ( IsNewClause ) {
			lits = new Disjunct;
			IsNewClause = false;
		}
		lits->push( (parsed_lit > 0) ? mkLit(var) : ~mkLit(var) );
    }
}
Minisat::Problem minisat22_wrapper::convert_to_problem(std::vector<std::vector<int>>& M) {
	Minisat::Problem cnf;
	Minisat::Disjunct *lits;
	for (int i = 0; i < M.size(); i++) {
		std::vector<int> tmp;
		tmp = M[i];
		lits = new Minisat::Disjunct;
		convertClause(tmp, *lits);
		cnf.push_back(lits);
	}
	return cnf;
}

void minisat22_wrapper :: printProblem(const Problem& p, std::ostream& out)
{
	for(size_t i = 0; i < p.size(); i++) {
		for(int j = 0; j < p[i]->size(); j++) {
			Lit& lit = (*p[i])[j];
			out << (sign(lit)?"-":"") << var(lit)+1 << (j+1==p[i]->size()?" 0\n":" ");
		}
	}
}

int minisat22_wrapper::minisat_solve_inc(Problem &CNF_in, std::vector<std::vector<int>> assumpts_vectors, std::vector<int> &sat_solution)
{
	static Solver* solver;
	double time = cpuTime();

	double initial_time = cpuTime();

	// КНФ считываем 1 раз

	// передача КНФ решателю, можно делать в цикле
	Solver S;
	S.verbosity = 2;
	S.addProblem(CNF_in);
	double parsed_time = cpuTime();

	if (S.verbosity > 0) {
		printf("|  Number of variables:  %12d                                         |\n", S.nVars());
		printf("|  Number of clauses:    %12d                                         |\n", S.nClauses());
	}

	if (S.verbosity > 0) {
		printf("|  Parse time:           %12.2f s                                       |\n", parsed_time - initial_time);
		printf("|                                                                       |\n");
	}

	if (!S.simplify()) {
		if (S.verbosity > 0) {
			printf("===============================================================================\n");
			printf("Solved by unit propagation\n");
			//printStats(S);
			printf("\n");
		}
		printf("UNSATISFIABLE\n");
		printf("time %f", cpuTime() - time);
		//exit(20);
	}

	printf("|  after simplify:           %12.2f s                                       |\n", parsed_time - initial_time);

	//S.start_activity = 1;
	//S.resetIntervalVarActivity( 578, 777 );
	S.print_learnts = true;

	bool flag = true;
	std::vector<int> current_ss;
	int k = 0;
	while (flag == true) {
		vec<Lit> assumptions;
		convertClause(assumpts_vectors[k], assumptions);
		k++;
		std::cout<<std::endl<<"k="<<k;
		//vec<Lit> dummy;
		lbool ret = S.solveLimited(assumptions);

		if (S.verbosity > 0) {
			//printStats(S);
			printf("\n");
		}
		//printf(ret == l_True ? "SATISFIABLE\n" : ret == l_False ? "UNSATISFIABLE\n" : "INDETERMINATE\n");

		if (ret == l_True)
			printf("SAT\n");
		else if (ret == l_False)
			printf("UNSAT\n");
		else
			printf("INDET\n");


		printf("time %f", cpuTime() - time);

		if (ret == l_True) {
			current_ss.clear();
			for (int i = 0; i < S.nVars(); ++i)
				current_ss.push_back((S.model[i] == l_True) ? 1 : 0);
		}

		if (k > assumpts_vectors.size() - 1) { flag = false; }
		if (ret != l_True) { flag = false; }

		//return (ret == l_True ? 10 : ret == l_False ? 20 : 0);

		//debug
		//std::ofstream out("out.cnf", std::ios::out);
		//printProblem(cnf, out);
		//out.close();
	}
	sat_solution = current_ss;
	return 0;
}


int minisat22_wrapper::minisat_solve(Problem &CNF_in, std::vector<int> assumpts, std::vector<int> &sat_solution)
{
	static Solver* solver;
	double time = cpuTime();

	double initial_time = cpuTime();

	// КНФ считываем 1 раз

	// передача КНФ решателю, можно делать в цикле
	Solver S;
	//S.verbosity = 1;
	S.verbosity = 0;
	S.addProblem(CNF_in);
	double parsed_time = cpuTime();

	if (S.verbosity > 0) {
		printf("|  Number of variables:  %12d                                         |\n", S.nVars());
		printf("|  Number of clauses:    %12d                                         |\n", S.nClauses());
	}

	if (S.verbosity > 0) {
		printf("|  Parse time:           %12.2f s                                       |\n", parsed_time - initial_time);
		printf("|                                                                       |\n");
	}

	if (!S.simplify()) {
		if (S.verbosity > 0) {
			printf("===============================================================================\n");
			printf("Solved by unit propagation\n");
			//printStats(S);
			printf("\n");
		}
		printf("UNSATISFIABLE\n");
		printf("time %f", cpuTime() - time);
		//exit(20);
	}

	printf("|  after simplify:           %12.2f s                                       |\n", parsed_time - initial_time);

	//S.start_activity = 1;
	//S.resetIntervalVarActivity( 578, 777 );
	S.print_learnts = true;

	vec<Lit> assumptions;
	convertClause(assumpts, assumptions);
	//vec<Lit> dummy;
	
	/*S.core_len = 64;	
	S.start_activity=1;
	S.resetVarActivity();
	
	*/

	lbool ret = S.solveLimited(assumptions);	
	if (S.verbosity > 0) {
		//printStats(S);
		printf("\n");
	}
	//printf(ret == l_True ? "SATISFIABLE\n" : ret == l_False ? "UNSATISFIABLE\n" : "INDETERMINATE\n");

	if (ret == l_True)
		printf("SAT\n");
	else if (ret == l_False)
		printf("UNSAT\n");
	else
		printf("INDET\n");


	printf("time %f", cpuTime() - time);
	
	if (ret == l_True) {
		for (int i = 0; i < S.nVars(); ++i)
			sat_solution.push_back((S.model[i] == l_True) ? 1 : 0);		
	}

	return (ret == l_True ? 10 : ret == l_False ? 20 : 0);

	//debug
	//std::ofstream out("out.cnf", std::ios::out);
	//printProblem(cnf, out);
	//out.close();

	return 0;
}



int minisat22_wrapper::minisat_solve_noout(Problem &CNF_in, std::vector<int> assumpts, std::vector<int> &sat_solution)
{
	static Solver* solver;
	double time = cpuTime();

	double initial_time = cpuTime();

	// КНФ считываем 1 раз

	// передача КНФ решателю, можно делать в цикле
	Solver S;
	//S.verbosity = 1;
	S.verbosity = 0;
	S.addProblem(CNF_in);
	double parsed_time = cpuTime();

	//S.start_activity = 1;
	//S.resetIntervalVarActivity( 578, 777 );
	S.print_learnts = true;

	vec<Lit> assumptions;
	convertClause(assumpts, assumptions);
	//vec<Lit> dummy;

	/*S.core_len = 64;
	S.start_activity=1;
	S.resetVarActivity();

	*/

	lbool ret = S.solveLimited(assumptions);

	//printf(ret == l_True ? "SATISFIABLE\n" : ret == l_False ? "UNSATISFIABLE\n" : "INDETERMINATE\n");

	if (ret == l_True) {
		for (int i = 0; i < S.nVars(); ++i)
			sat_solution.push_back((S.model[i] == l_True) ? 1 : 0);
	}

	//return (ret == l_True ? 10 : ret == l_False ? 20 : 0);
	S.checkGarbage();
	//debug
	//std::ofstream out("out.cnf", std::ios::out);
	//printProblem(cnf, out);
	//out.close();

	return 0;
}



int minisat22_wrapper::minisat_solve_find_tv(Problem &CNF_in, int n, std::vector<int> assumpts, std::vector<std::vector<int>> &sat_solutions)
{
	static Solver* solver;
	double time = cpuTime();

	double initial_time = cpuTime();

	// КНФ считываем 1 раз

	// передача КНФ решателю, можно делать в цикле
	Solver S;
	//S.verbosity = 1;
	S.verbosity = 0;
	S.addProblem(CNF_in);
	double parsed_time = cpuTime();

	//S.start_activity = 1;
	//S.resetIntervalVarActivity( 578, 777 );
	S.print_learnts = true;

	vec<Lit> assumptions;
	convertClause(assumpts, assumptions);
	//vec<Lit> dummy;

	/*S.core_len = 64;
	S.start_activity=1;
	S.resetVarActivity();

	*/	
	bool b = true;
	while (b){
		lbool ret = S.solveLimited(assumptions);
		//printf(ret == l_True ? "SATISFIABLE\n" : ret == l_False ? "UNSATISFIABLE\n" : "INDETERMINATE\n");
		std::vector<int> sat_solution;
		if (ret == l_True) {
			std::vector<int> lrnt;
			for (int i = 0; i < n*n; ++i)
			{
				if (S.model[i] == l_True) {
					sat_solution.push_back(i%n);
					lrnt.push_back(-(i+1));
				}
			}
		/*	for (auto i = 0; i < sat_solution.size(); i++) {
				std::cout << sat_solution[i] << " ";
			}
			std::cout << std::endl;*/
			sat_solutions.push_back(sat_solution);
			vec<Lit> new_clause;
			convertClause(lrnt, new_clause);
			S.addClause(new_clause);				
		}
		else {
			b = false;
		}

	}
	//return (ret == l_True ? 10 : ret == l_False ? 20 : 0);

	//debug
	//std::ofstream out("out.cnf", std::ios::out);
	//printProblem(cnf, out);
	//out.close();
	S.checkGarbage();
	return 0;
}


int minisat22_wrapper::minisat_solve_incremental(Problem &CNF_in, std::vector<std::vector<int>> &sat_solutions) {
	static Solver* solver;
	double time = cpuTime();

	double initial_time = cpuTime();

	// КНФ считываем 1 раз

	// передача КНФ решателю, можно делать в цикле
	Solver S;
	S.verbosity = 1;
	S.addProblem(CNF_in);	
	double parsed_time = cpuTime();

	if (S.verbosity > 0) {
		printf("|  Number of variables:  %12d                                         |\n", S.nVars());
		printf("|  Number of clauses:    %12d                                         |\n", S.nClauses());
	}

	if (S.verbosity > 0) {
		printf("|  Parse time:           %12.2f s                                       |\n", parsed_time - initial_time);
		printf("|                                                                       |\n");
	}

	if (!S.simplify()) {
		if (S.verbosity > 0) {
			printf("===============================================================================\n");
			printf("Solved by unit propagation\n");
			//printStats(S);
			printf("\n");
		}
		printf("UNSATISFIABLE\n");
		printf("time %f", cpuTime() - time);
		//exit(20);
	}

	printf("|  after simplify:           %12.2f s                                       |\n", parsed_time - initial_time);

	
	S.print_learnts = true;
	
	vec<Lit> dummy;
	bool cont = true;
	while (cont == true) {

		lbool ret = S.solveLimited(dummy);
		if (S.verbosity > 0) {
			//printStats(S);
			printf("\n");
		}
		//printf(ret == l_True ? "SATISFIABLE\n" : ret == l_False ? "UNSATISFIABLE\n" : "INDETERMINATE\n");

		if (ret == l_True){
			printf("SAT\n");
			std::vector<int> ss;
			printf("time %f", cpuTime() - time);
			time = cpuTime();

			if (ret == l_True) {
				for (int i = 0; i < S.nVars(); ++i)
					ss.push_back((S.model[i] == l_True) ? 1 : 0);
			}

			//		procedure that constructs clause forbidding found LS
			std::vector<int> restrict;
			//std::cout << "restriction \n";
			for (int i = 0; i < 2000; i++) {
				if (ss[i] > 0) {
					restrict.push_back(-(i+1));
					//std::cout << -(1+i) << " ";
				}
			}
			vec<Lit> new_clause;
			convertClause(restrict, new_clause);
			S.addClause(new_clause);
		}
		else if (ret == l_False) {
			printf("UNSAT\n");
			cont = false;
		}
		else{
			printf("INDET\n");
			cont = false;
		}

	

		//return (ret == l_True ? 10 : ret == l_False ? 20 : 0);

		//debug
		//std::ofstream out("out.cnf", std::ios::out);
		//printProblem(cnf, out);
		//out.close();
	}
	return 0;

}


int minisat22_wrapper::minisat_solve_file(int argc, char* argv[])
{
	static Solver* solver;
#ifdef _DEBUG
	argc = 3;
	argv[1] = "bivium_template.cnf";
	argv[2] = "out";
#endif
	double time = cpuTime();
	std::ifstream in(argv[1], std::ios::in);
	if (!in.is_open())
		printf("ERROR! Could not open file: %s\n", argc == 1 ? "<stdin>" : argv[1]), exit(1);

	double initial_time = cpuTime();

	// КНФ считываем 1 раз
	Problem cnf;
	parse_DIMACS_to_problem(in, cnf);
	in.close();
	double parsed_time = cpuTime();

	// передача КНФ решателю, можно делать в цикле
	Solver S;
	S.verbosity = 2;
	S.addProblem(cnf);

	FILE* res = (argc >= 3) ? fopen(argv[2], "wb") : NULL;

	if (S.verbosity > 0) {
		printf("|  Number of variables:  %12d                                         |\n", S.nVars());
		printf("|  Number of clauses:    %12d                                         |\n", S.nClauses());
	}

	if (S.verbosity > 0) {
		printf("|  Parse time:           %12.2f s                                       |\n", parsed_time - initial_time);
		printf("|                                                                       |\n");
	}

	if (!S.simplify()) {
		if (res != NULL) fprintf(res, "UNSAT\n"), fclose(res);
		if (S.verbosity > 0) {
			printf("===============================================================================\n");
			printf("Solved by unit propagation\n");
			//printStats(S);
			printf("\n");
		}
		printf("UNSATISFIABLE\n");
		printf("time %f", cpuTime() - time);
		exit(20);
	}

	printf("|  after simplify:           %12.2f s                                       |\n", parsed_time - initial_time);

	//S.start_activity = 1;
	//S.resetIntervalVarActivity( 578, 777 );
	S.print_learnts = true;

	vec<Lit> dummy;
	lbool ret = S.solveLimited(dummy);
	if (S.verbosity > 0) {
		//printStats(S);
		printf("\n");
	}
	//printf(ret == l_True ? "SATISFIABLE\n" : ret == l_False ? "UNSATISFIABLE\n" : "INDETERMINATE\n");

	if (ret == l_True)
		printf("SAT\n");
	else if (ret == l_False)
		printf("UNSAT\n");
	else
		printf("INDET\n");

	if (res != NULL) {
		if (ret == l_True) {
			fprintf(res, "SAT\n");
			for (int i = 0; i < S.nVars(); i++)
				if (S.model[i] != l_Undef)
					fprintf(res, "%s%s%d", (i == 0) ? "" : " ", (S.model[i] == l_True) ? "" : "-", i + 1);
			fprintf(res, " 0\n");
		}
		else if (ret == l_False)
			fprintf(res, "UNSAT\n");
		else
			fprintf(res, "INDET\n");
		fclose(res);
	}
	printf("time %f", cpuTime() - time);

#ifdef NDEBUG
	exit(ret == l_True ? 10 : ret == l_False ? 20 : 0);     // (faster than "return", which will invoke the destructor for 'Solver')
#else
	return (ret == l_True ? 10 : ret == l_False ? 20 : 0);
#endif

	//debug
	//std::ofstream out("out.cnf", std::ios::out);
	//printProblem(cnf, out);
	//out.close();

	return 0;
}