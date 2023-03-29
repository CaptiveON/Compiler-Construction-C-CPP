#ifndef _PARSER_H_
#define _PARSER_H_
#include "lexer.h"
struct funcDeclaration 
{
    string funcName;
    int lineNo;
};
class parser
{
    lexer _lexer;
    int varAddress;
    int lineNo;
    int tempCall;
    vector<string> tac;
    vector<funcDeclaration> func;
    
public:
    void syntax_error();
    token expect(TokenType expected_type);
    parser(const char filename[]);
    void readAndPrintAllInput();
    void resetPointer();
    int pointerCurrentPlace();
    int noOfToken();

    //Parse Tree Functions
    void functions();
    void function();
    void gvarasnleftfact();
    void params();
    void param();
    void multiparamcomma();
    void statements();
    void statement();
    void comment();
    string relationaloperator();
    void ifconditionleftfact(int);
    void elseconditionleftfact();
    string varasnleftfact();
    string asnleftfact();
    string asnvalueleftfact();
    void expoccurences();
    void expoccurence();
    void multiexpcomma();
    void vartype();
    string displayleftfact();
    string displaylineleftfact();
    string exp();
    string exp1();
    // string exp1(string);
    string lowpre();
    string lowpre1();
    // string lowpre1(string);
    string highpre();

    //Translation Scheme
    string tempGen();
    void emit(string);
    void saveTAC();
    void backPatch(int, string);
    string simName(string);
    int findFunc(string);
    bool findMain();
};
#endif
