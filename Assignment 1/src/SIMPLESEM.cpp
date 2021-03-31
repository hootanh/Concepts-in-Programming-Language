/***************************************
* SIMPLESEM.cpp
*
* Student Name: Hootan Hosseinzadeganbushehri
**************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <cstring>

using namespace std;

class SIMPLESEM {
public:
	SIMPLESEM(char *sourceFile);
	void runProgram();

private:
	std::ifstream codeIn;
	std::ifstream inputIn;
	std::ofstream outFile;
	static int nextChar; //contains the character(or -1==EOF)
	const static int EOF_ = -1; //int value for End of File

	void printRule(const char*);

	void parseProgram();
	void parseStatement();
	void parseJump();
	void parseSet();
	void parseJumpt();
	void parseExpr();
	void parseTerm();
	void parseFactor();
};


SIMPLESEM::SIMPLESEM(char *sourceFile) {
	codeIn.open(sourceFile, std::fstream::in);
	if (codeIn.fail()) {
		std::cerr << "init: Errors accessing source file "
			<< sourceFile << std::endl;
		exit(-2);
	}

	/*inputIn.open("input.txt", std::fstream::in);
	if (inputIn.fail()) {
		std::cerr << "init: Errors accessing input file input.txt" << std::endl;
		exit(-2);
	}*/

	outFile.open((std::string(sourceFile) + ".out").c_str(), std::fstream::out);
	if (outFile.fail()) {
		std::cerr << "init: Errors accessing output file "
			<< std::string(sourceFile) + ".out" << std::endl;
		exit(-2);
	}


}

void SIMPLESEM::runProgram()
{
	parseProgram();
	codeIn.close();
	outFile.close();
}

void SIMPLESEM::printRule(const char* rule)
{
	outFile << rule << std::endl;
}

void SIMPLESEM::parseProgram() {
	printRule("Program");

	parseStatement();
	while (nextChar != EOF_) {
		parseStatement();
	}

}

void SIMPLESEM::parseStatement() {
	printRule("Statement");
	string statement;
	codeIn >> statement;
	if (statement == "jump")
	{
		parseJump();
	}
	else if (statement == "set")
	{
		parseSet();
	}
	else if (statement == "jumpt")
	{
		parseJumpt();
	}
	else if (statement == "halt")
	{
		nextChar = -1;
	}
	

	//Complete productions here
}

void SIMPLESEM::parseJump()
{
	printRule("Jump");
	parseExpr();
	if (nextChar != -1)
		codeIn.putback((char)(nextChar));
}

void SIMPLESEM::parseSet()
{
	printRule("Set");
	char c;

	codeIn >> c;
	if (c != 'w' && c != 'r')
	{
		codeIn.putback(c);
		parseExpr();
	}
	else
	{
		string statement;
		codeIn >> statement;
	}
	codeIn >> c;
	if (c != 'w' && c != 'r')
	{
		codeIn.putback(c);
		parseExpr();
		if (nextChar != -1)
			codeIn.putback((char)(nextChar));
	}
	else
	{
		string statement;
		codeIn >> statement;
	}
}

void SIMPLESEM::parseJumpt()
{
	printRule("Jumpt");
	parseExpr();
	parseExpr();
	codeIn.putback((char)(nextChar));
	string statement;
	codeIn >> statement;
	parseExpr();
	if (nextChar != -1)
		codeIn.putback((char)(nextChar));
}


void SIMPLESEM::parseExpr()
{
	printRule("Expr");
	parseTerm();
	while (1) {
		if (nextChar == '+' || nextChar == '-')
			parseTerm();
		else
			break;
	}
}

void SIMPLESEM::parseTerm()
{
	printRule("Term");
	parseFactor();
	while (1) {
		if (nextChar == '*' || nextChar == '/' || nextChar == '%')
			parseFactor();
		else
			break;
	}
}

void SIMPLESEM::parseFactor()
{
	printRule("Factor");
	char c;
	codeIn >> c;
	if (c == 'D')
	{
		codeIn >> c;
		if (c == '[')
			parseExpr();
	}
	else if (c == '(')
	{
		parseExpr();
	}
	else if (c >= '0' && c <= '9')
	{
		while (c >= '0' && c <= '9' && !codeIn.eof())
		{
			codeIn >> c;
		}

		nextChar = int(c);

		if (codeIn.eof())
			nextChar = -1;
		printRule("Number");
	}
	if (c == ']' || c == ')')
	{
		codeIn >> c;
		while (c == ']' || c == ')')
		{
			codeIn >> c;
		}
		nextChar = int(c);
	}
}

int SIMPLESEM::nextChar = -1;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "init: This program requires an input "
			<< "filename as a parameter " << std::endl;
		exit(-2);
	}
	SIMPLESEM s(argv[1]);
	s.runProgram();
	return 0;
}
