/***************************************
 * INTERPRETER.cpp
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

#define DATA_SEG_SIZE 100

class INTERPRETER {
public:
  INTERPRETER(char *sourceFile);
  void runProgram();

private:
  std::ifstream codeIn;
  std::ifstream inputIn;
  std::ofstream outFile;

  std::vector<std::string> C;
	int D[DATA_SEG_SIZE];
	int PC;
  std::string IR;
	bool run_bit;

  unsigned curIRIndex;
  void printDataSeg();
  void fetch();
  void incrementPC();
  void execute();
  void write(int source);
  int read();
  bool accept(char c);
  bool accept(const char *s);
  void expect(char c);
  void expect(const char *s);
  void skipWhitespace();
  void parseStatement();
  void parseSet();
  void parseJump();
  void parseJumpt();
  int parseExpr();
  int parseTerm();
  int parseFactor();
  int parseNumber();
  void syntaxError();
};


INTERPRETER::INTERPRETER(char *sourceFile) {
  codeIn.open(sourceFile, std::fstream::in);
  if (codeIn.fail()) {
    std::cerr << "init: Errors accessing source file "
              << sourceFile << std::endl;
    exit(-2);
  }

  inputIn.open("input.txt", std::fstream::in);
  if (inputIn.fail()) {
    std::cerr << "init: Errors accessing input file input.txt" << std::endl;
    exit(-2);
  }

  outFile.open((std::string(sourceFile) + ".out").c_str(), std::fstream::out);
  if (outFile.fail()) {
    std::cerr << "init: Errors accessing output file "
              << std::string(sourceFile) + ".out" << std::endl;
    exit(-2);
  }

  while (codeIn.good()) {
    std::string line;
    std::getline(codeIn, line);
    C.push_back(line);
  }

  for (int i=0; i<DATA_SEG_SIZE ;i++) {
    D[i]=0;
  }
  PC = 0;
  run_bit = true;
}

void INTERPRETER::runProgram() {
	while (run_bit)
	{
		fetch();
		incrementPC();
		execute();
	}

  printDataSeg();
}

void INTERPRETER::printDataSeg() {
  outFile << "Data Segment Contents" << std::endl;
  for(int i=0; i < DATA_SEG_SIZE; i++){
    outFile << i << ": " << D[i] << std::endl;
  }
}

void INTERPRETER::fetch() {
	IR = C[PC];
	curIRIndex = 0;
}

void INTERPRETER::incrementPC() {
	PC = PC + 1;
}

void INTERPRETER::execute() {
	parseStatement();
}

void INTERPRETER::write(int source){
  outFile << source << std::endl;
}

int INTERPRETER::read() {
  int value;
  inputIn >> value;
  return value;
}

bool INTERPRETER::accept(char c) {
  if (curIRIndex >= IR.length())
    return false;

  if (IR[curIRIndex] == c) {
    curIRIndex++;
    skipWhitespace();
    return true;
  } else {
    return false;
  }
}

bool INTERPRETER::accept(const char *s) {
  unsigned s_len = strlen(s);

  if (curIRIndex+s_len > IR.length())
    return false;

  for (unsigned i = 0; i < s_len; ++i) 
  {
    if (IR[curIRIndex+i] != s[i]) {
      return false;
    }
  }

  curIRIndex += s_len;
  skipWhitespace();
  return true;
}

void INTERPRETER::expect(char c) {
  if (!accept(c))
    syntaxError();
}

void INTERPRETER::expect(const char *s) {
  if (!accept(s))
    syntaxError();
}

void INTERPRETER::skipWhitespace() {
  while (curIRIndex < IR.length() && isspace(IR[curIRIndex]))
    curIRIndex++;
}

void INTERPRETER::parseStatement() {
  if (accept("halt")) {
	  run_bit = false;
	  //return;
  }
  else if(accept("set"))
    parseSet();
  else if(accept("jumpt"))
    parseJumpt();
  else if(accept("jump"))
    parseJump();
}

void INTERPRETER::parseSet() {
  int dig = 0;
  if (!accept("write"))
  {
	  dig = parseExpr();
	  expect(',');
	  if (!accept("read"))
		  D[dig] = parseExpr();
	  else
	  {
		  D[dig] = read();
	  }
  }
  else
  {
	  expect(',');
	  if (!accept("read"))
		  write(parseExpr());
	  else
	  {
		  write(read());
	  }
  }
}

int INTERPRETER::parseExpr() {
  int dig;
  dig = parseTerm();
  while (true) 
  {
	  if (accept('+'))
		  dig = dig + parseTerm();
	  else if (accept('-'))
		  dig = dig - parseTerm();
	  else
		  break;
  }
  return dig;
}

int INTERPRETER::parseTerm(){
  int dig;
  dig = parseFactor();

  while (true)
  {
	  if (accept('*'))
		  dig = dig * parseFactor();
	  else if (accept('/'))
		  dig = dig / parseFactor();
	  else if (accept('%'))
		  dig = dig % parseFactor();
	  else
		  break;
  }
  return dig;
}

int INTERPRETER::parseFactor() {
  int dig=-1;
  if (accept("D[")) {
     dig = D[parseExpr()];

    expect(']');
  } else if (accept('(')) {
    dig = parseExpr();

    expect(')');
  } else {
    dig= parseNumber();
  }
  return dig;
}

int INTERPRETER::parseNumber() {
  if (curIRIndex >= IR.length())
    syntaxError();

  if (IR[curIRIndex] == '0') 
  {
    curIRIndex++;
    skipWhitespace();
    return 0;
  } 
  else if (isdigit(IR[curIRIndex])) 
  {
	  int dig = IR[curIRIndex] - '0';
	  curIRIndex++;
    while (curIRIndex < IR.length() &&
           isdigit(IR[curIRIndex])) 
	{
		dig = (dig * 10) + (IR[curIRIndex] - '0');
		curIRIndex++;
    }
    skipWhitespace();
	return dig;
  } else 
  {
    syntaxError();
  }
}

void INTERPRETER::parseJump() {
  PC = parseExpr();
}

void INTERPRETER::parseJumpt() {
  int a, b;
  int PC_t = parseExpr();

  expect(',');

  a = parseExpr();

  if (accept("!="))
  {
	  b = parseExpr();
	  if (a != b)
		  PC = PC_t;
  }
  else if (accept("=="))
  {
	  b = parseExpr();
	  if (a == b)
		  PC = PC_t;
  }
  else if (accept(">="))
  {
	  b = parseExpr();
	  if (a >= b)
		  PC = PC_t;
  }
  else if (accept("<="))
  {
	  b = parseExpr();
	  if (a <= b)
		  PC = PC_t;
  }
  else if (accept(">"))
  {
	  b = parseExpr();
	  if (a > b)
		  PC = PC_t;
  }
  else if (accept("<"))
  {
	  b = parseExpr();
	  if (a < b)
		  PC = PC_t;
  }
  else{
	  syntaxError();

  }
}

void INTERPRETER::syntaxError() {
  std::cerr << "Syntax Error!" << std::endl;
  exit(-1);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "init: This program requires an input "
              << "filename as a parameter " << std::endl;
    exit(-2);
  }
  INTERPRETER i(argv[1]);
  i.runProgram();
}
