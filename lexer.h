#ifndef _LEXER_H_
#define _LEXER_H_
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cctype>
using namespace std;

//all your tokens goes here
enum class TokenType
{
	END_OF_FILE,
	ERROR,
	FUNCTION,
	INT,
	CHAR,
	FLOAT,
	IF,
	ELSE,
	DO,
	UNTIL,
	THEN,
	READ,
	DISPLAY,
	DISPLAYLINE,
	RETURN,
	EXECUTE,
	NUM,
	ID,
	VAR,
	CMA, //COMMA
	CLN, //COLON
	SCLN, //SEMI COLON
	DLR, //DOLLAR
	OPTS, //OPENING PARENTHESIS
	CPTS, //CLOSING PARENTHESIS
	ADD, //ADD
	SBT, //SUBTRACT
	MUL, //MULTIPLY
	DIV, //DIVIDE
	MOD, //MODULUS
	SBL, //START OF BLOCK
	CBL, //END OF BLOCK
	ASN, //ASSIGNMENT
	FPASN, //FUNCTION POINTER ASSIGNMENT OPERATOR
	EQL, //EQUAL TO
	NEQ, //NOT EQUAL TO
	LTN, //LESS THAN
	LTE, //LESS THAN EQUAL TO
	GTR, //GREATER THAN
	GTE, //GREATER THAN EQUAL TO
	CMT, //COMMENT
	DQSTR,  //DOUBLE QUOTATION STRING
	SQSTR //SINGLE QUOTATION STRING
};
//structure of a token 
struct token
{
	int lineNo=0;
	string lexeme;
	TokenType tokenType;//enum type
	//constructor
	token(int lineNo, string lexeme, TokenType tokenType);
	//constructor default
	token();
	void Print();
};
class lexer
{
	vector<char> stream;  //used for storing file sample_code.ol content
	vector<token> tokens; //vector to store all (tokens,lexeme) pairs
	void Tokenize();//populates tokens vector
	int index;

public:
	lexer();
	lexer(const char filename[]);
	void printRaw();//just print everything as it is present in file
	token getNextToken();//get next token
	void resetPointer();//reset pointer to start getting tokens from start
	int getCurrentPointer();//get where current pointer in tokens vector is
	void setCurrentPointer(int pos);//move current pointer to wherever
	token peek(int);//peek the next token
	int noOfToken();
	token getToken(int);
};

#endif // !_LEXER_H
