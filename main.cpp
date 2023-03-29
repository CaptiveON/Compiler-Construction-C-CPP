#include <iostream>
#include "parser.h"
using namespace std;

int main(int argc, char* argv[])
{
	//asking for file name as command line arguments
	if (argc == 2)
	{
		// lexer _lexer(argv[1]);
		// token t;
		// t = _lexer.getNextToken();
		// while (t.tokenType != TokenType::END_OF_FILE)
		// {
		// 	t.Print();
		// 	t = _lexer.getNextToken();
		// }
		parser _parser(argv[1]);
		cout<<"\n-----------------------------------------\n";
		cout<<"TOKEN LEXEME Pairs:";
		cout<<"\n-----------------------------------------\n";
		_parser.readAndPrintAllInput();
		cout<<"\n-----------------------------------------\n";
		cout<<"Parser Function Calls:";
		cout<<"\n-----------------------------------------\n";
		_parser.resetPointer();
		_parser.functions();

		bool mainFun = _parser.findMain(); //There should exist a main function

		if(_parser.pointerCurrentPlace() != (_parser.noOfToken() - 1))
		{
			cout<<"\n-----------------------------------------\n";
			cout<<"Current: "<<_parser.pointerCurrentPlace()<<"\n";
			cout<<"Total: "<<_parser.noOfToken()-1;
			cout<<"\n--------------!ERROR!-AT-EOF--------------\n";
		}
		else if (mainFun == false)
		{
			cout<<"\n-----------------------------------------\n";
			cout<<"\n--------!ERROR!-NO-MAIN-FUNCTION---------\n";
		}
		else
		{
			cout<<"\n-----------------SUCCESS-----------------\n";
			cout<<"Current Token Index: "<<_parser.pointerCurrentPlace()<<"\n";
			cout<<"Total no. of Tokens: "<<_parser.noOfToken()-1;
			cout<<"\n-----------------SUCCESS-----------------\n";
			_parser.saveTAC();
		}
	}
	else if (argc > 2)
	{ //argument limit exceeds
		cout << "Too many arguments" << endl;
	}
	else //if file name is'nt given
	{
		cout << "Please provide a file name" << endl;
	}
	return 0;
}
