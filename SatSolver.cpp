#include "SatSolver.h"
using namespace std;

bool SatSolver::MakeQuery()
{
	// Create a vector that contains all the elements in 'clauses' and 'query_clauses'.
	vector<Clause> all_clauses = clauses;
	all_clauses.insert(all_clauses.end(), query_clauses.begin(), query_clauses.end());
	
	// Find the literal with the largest absolute value. 
	int max_var = 0;
	for (int i = 0; i < all_clauses.size(); i++)
		for (int j = 0; j < all_clauses[i].size(); j++)
			if (abs(all_clauses[i][j]) > max_var)
				max_var = abs(all_clauses[i][j]);

	// Print out the CNF query file.
	ofstream dimacs;
	dimacs.open("query.cnf");
	dimacs<<"c This DIMACS format CNF file was generated by SatSolver.cpp"<<endl;
	dimacs<<"c Do not edit."<<endl;
	
	dimacs<<"p cnf "<<max_var<<" "<<all_clauses.size()<<endl;
	
	for (int i = 0; i < all_clauses.size(); i++)
	{
		for (int j = 0; j < all_clauses[i].size(); j++)
			dimacs<<all_clauses[i][j]<<" ";
		
		dimacs<<"0"<<endl;
	}
	dimacs.close();
	
	// Call zChaff with the query file and read its result.
	FILE* pipe = popen("./zchaff query.cnf", "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    	
	int found = result.find("RESULT:\tSAT");
	if (found != string::npos)
		return true;
	
	return false;
}

int SatSolver::TestLiteral(Literal l)
{
	// Calls the SAT solver twice, once with l, once with -l and determines whether l is true (1), false (-1), or can take both values (0).
	
	int result = kUnknown;
	
	ClearQueryClauses();
	Clause c;
	c.push_back(l);
	AddQueryClause(c);
	
	if (!MakeQuery())
		result = kFalse;
	else
	{
		ClearQueryClauses();
		Clause c;
		c.push_back(-l);
		AddQueryClause(c);
		
		if (!MakeQuery())
			result = kTrue;
	}
	
	ClearQueryClauses();
	return result;
}
