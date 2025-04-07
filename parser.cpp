/* Implementation of Recursive-Descent Parser
	for the Simple Ada-Like (SADAL) Language
 * parser.cpp
 * Programming Assignment 2
 * Spring 2025
*/
#include <queue>
#include "parser.h"


map<string, bool> defVar;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;
	LexItem temp;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			temp = pushed_token;
			cout << "Parsing pushed token: " << temp;

			return pushed_token;
		}
		temp = getNextToken(in, line);
		cout << "Parsing token: " << temp;
		return temp;
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

// Prog ::= PROCEDURE ProcName IS ProcBody
bool Prog(istream& in, int& line) {
	LexItem tok; 
	cout << "Line" << line << "(in prog)" << endl;
	tok = Parser::GetNextToken(in, line); 

	if (tok != PROCEDURE) 
	{
		ParseError(line, "Incorrect compilation file.");
		return false;
	}
	tok = Parser::GetNextToken(in, line);
	
	if (tok != IDENT) 
	{
		ParseError(line, "Missing Procedure Name.");
		return false;
	}
	tok = Parser::GetNextToken(in, line);

	if (tok != IS) 
	{
		ParseError(line, "Missing an is.");
		return false;
	}

	if (!ProcBody(in, line)) 
	{
		ParseError(line, "Incorrect Procedure Definition.");
		return false;
	}
    return true;
}

bool ProcBody(istream& in, int& line) {
	LexItem tok;
	cout << "Line" << line << "(in procbody)" << endl;
	
	if (!DeclPart(in, line)) {
		ParseError(line, "Incorrect procedure body.");
		return false;
	}
	tok = Parser::GetNextToken(in, line); 

	if (tok != BEGIN) {
		ParseError(line, "Incorrect procedure body.");
		return false;
	}
	if (!StmtList(in, line)) {
		ParseError(line, "Incorrect procedure body.");
		return false;
	}

	tok = Parser::GetNextToken(in, line);

	if (tok != END) {
		ParseError(line, "Incorrect procedure body.");
		return false;
	}
	tok = Parser::GetNextToken(in, line);

	if (tok != IDENT) {
		ParseError(line, "Incorrect procedure body.");
		return false;
	}
	tok = Parser::GetNextToken(in, line); 

	
	if (tok != SEMICOL) {
		ParseError(line, "Incorrect procedure body.");
		return false;
	}
	return true;
}

//GIVEN FUNCTION - DON'T TOUCH
//DeclPart ::= DeclStmt { DeclStmt }
bool DeclPart(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	cout << "Line" << line << "(in declpart)" << endl;
	status = DeclStmt(in, line);
	if(status)
	{
		tok = Parser::GetNextToken(in, line);
		if(tok == BEGIN )
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else 
		{
			Parser::PushBackToken(tok);
			status = DeclPart(in, line);
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Declaration Part.");
		return false;
	}
	return true;
}

//DeclStmt ::= IDENT {, IDENT } : [CONSTANT] Type [(Range)] [:= Expr] ;
bool DeclStmt(istream& in, int& line) {
	LexItem tok;
	cout << "Line" << line << "(in declstmt)" << endl;
	
	// 1st IDENT
	tok = Parser::GetNextToken(in, line);
	if (tok != IDENT) 
	{
		ParseError(line, "Incorrect declaration statement.");
		return false;
	}
	else
	{
		defVar[tok.GetLexeme()] = true;
	}

	// Loop through optional IDENTs
	tok = Parser::GetNextToken(in, line);
	bool status = true;
	while (status)
	{
		if (tok == COMMA)
		{	
			tok = Parser::GetNextToken(in, line);
			if (tok != IDENT) 
			{
				ParseError(line, "Incorrect declaration statement.");
				return false;
			}
			if (defVar.count(tok.GetLexeme()))
			{
				ParseError(line, "Variable redefinition.");
				return false;
			}
			defVar[tok.GetLexeme()] = true;
			tok = Parser::GetNextToken(in, line);
		}
		//End of variable declaration, break out of while loop
		else if (tok == COLON)
		{
			// cout << "in COLON conditional" << endl;
			tok = Parser::GetNextToken(in, line);
			status = false;
		}
		else 
		{
			ParseError(line, "Incorrect declaration statement.");
			return false;
		}
	}

	//Optional CONSTANT
	if (tok == CONST)
	{
		tok = Parser::GetNextToken(in, line);
	}

	// Required TYPE
	// PushBackToken so can re-evaluate the same token in Type()
	Parser::PushBackToken(tok);
	if (!Type(in, line))
	{
		ParseError(line, "Incorrect Declaration Type.");
		return false;		
	}
	tok = Parser::GetNextToken(in, line);
	
		
	//TODO: handle [(Range)]
	//TODO: handle [:= Expr]

	// Check for SEMICOL at end of DeclStmt
	if (tok != SEMICOL)
	{
		ParseError(line, "Missing semicolon at end of statement.");
		return false;
	}

	return true;
}

// Type ::= INTEGER | FLOAT | BOOLEAN | STRING | CHARACTER
bool Type(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in Type)" << endl;

	tok = Parser::GetNextToken(in, line);
	if (tok == INT || tok == FLOAT || tok == BOOL || tok == STRING || tok == CHAR)
	{	
		return true;
	}
	else
	{
		return false;
	}
}

// Expr ::= Relation {(AND | OR) Relation }
bool Expr(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in Expr)" << endl;

	return false;
}

// Relation ::= SimpleExpr [ ( = | /= | < | <= | > | >= ) SimpleExpr ]
bool Relation(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in Relation)" << endl;

	return false;
}

// SimpleExpr ::= STerm { ( + | - | & ) STerm }
bool SimpleExpr(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in SimpleExpr)" << endl;

	return false;
}

// STerm ::= [ ( + | - ) ] Term
bool STerm(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in STerm)" << endl;

	return false;
}

// Term ::= Factor { ( * | / | MOD ) Factor } 
bool Term(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in Term)" << endl;

	return false;
}

// Factor ::= Primary [** [(+ | -)] Primary ] | NOT Primary
bool Factor(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in Factor)" << endl;

	return false;
}

// Primary ::= Name | ICONST | FCONST | SCONST | BCONST | CCONST | (Expr)
bool Primary(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in Primary)" << endl;

	return false;
}

// Name ::= IDENT [ ( Range ) ] 
bool Name(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in Name)" << endl;

	return false;
}

// Range ::= SimpleExpr [. . SimpleExpr ]
bool Range(istream& in, int& line)
{
	LexItem tok;
	cout << "Line" << line << "(in Range)" << endl;

	return false;
}

//GIVEN FUNCTION - DON'T TOUCH
//StmtList ::= Stmt { Stmt }
bool StmtList(istream& in, int& line)
{
	bool status;
	LexItem tok;
	// cout << "in StmtList" << endl;
	cout << "Line" << line << "(in stmtlist)" << endl;
	status = Stmt(in, line);
	tok = Parser::GetNextToken(in, line);
	while(status && (tok != END && tok != ELSIF && tok != ELSE))
	{
		Parser::PushBackToken(tok);
		status = Stmt(in, line);
		tok = Parser::GetNextToken(in, line);
	}
	if(!status)
	{
		ParseError(line, "Syntactic error in statement list.");
		return false;
	}
	Parser::PushBackToken(tok); //push back the END token
	return true;
}

bool Stmt(istream& in, int& line) {
	cout << "Line" << line << "(in stmt)" << endl;
	return false;
}
