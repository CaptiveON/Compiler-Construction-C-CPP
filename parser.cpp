#include <iostream>
#include <fstream>
#include "parser.h"
using namespace std;
ofstream symboltable("symbol_table.txt");
ofstream tacSave("TAC.txt");
void parser::syntax_error()
{
    int pos = _lexer.getCurrentPointer();
    token t = _lexer.getToken(pos - 2);
    cout << "\n-----------!SYNTAX ERROR OCCURED!-----------\n";
    cout << "-----------!Line no: " << t.lineNo;
    cout << "\n-----------!SYNTAX ERROR OCCURED!-----------\n";
    exit(1);
}
token parser::expect(TokenType expected_type)
{
    token t = _lexer.getNextToken();
    if (t.tokenType != expected_type)
    {
        syntax_error();
    }
    return t;
}
//If there exist a main function
bool parser::findMain()
{
    bool mainFun = false;
    for (int i = 0; i < parser::func.size(); i++)
    {
        funcDeclaration temp = func[i];
        if(temp.funcName == "main"){
            mainFun = true;
        }
    }
    return mainFun;
}
parser::parser(const char filename[])
{
    _lexer = lexer(filename);

    // Variable Address in symbol table
    varAddress = 0;
    // TAC instruction number
    lineNo = 1;
    // Temp Variable Count
    tempCall = 1;

    symboltable << "------------------------------------------------------------------------------------------------------------------------------------------------\n";
    symboltable << "Variable Name"
                << "            |           "
                << "Variable Value"
                << "            |           "
                << "Variable Type"
                << "            |           "
                << "Variable Address\n";
    symboltable << "------------------------------------------------------------------------------------------------------------------------------------------------\n";
}
void parser::readAndPrintAllInput() // read and print allinputs (provided)
{
    token t;
    t = _lexer.getNextToken();
    while (t.tokenType != TokenType::END_OF_FILE)
    {
        t.Print();
        t = _lexer.getNextToken();
    }
}
void parser::resetPointer()
{
    _lexer.resetPointer();
}
int parser::pointerCurrentPlace()
{
    return _lexer.getCurrentPointer();
}
int parser::noOfToken()
{
    return _lexer.noOfToken();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// CFG LL(1) /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// functions                    --> function functions | ^

// function                     --> ID CLN vartype FPASN FUNCTION OPTS params CPTS SBL statements CBL |
//                                  VAR CLN vartype gvarasnleftfact |
//                                  CMT

// gvarasnleftfact              --> ASN exp SCLN | SCLN

// params                       --> param params | ^

// param                        --> VAR CLN vartype multiparamcomma

// multiparamcomma              --> CMA param | ^

// statements                   --> statement statements | ^

/*
   statement                    --> IF OPTS exp relationaloperator exp OPTS THEN SBL statements CBL ifconditionleftfact |
                                    VAR varasnleftfact |
                                    DO UNTIL OPTS exp relationaloperator exp CPTS SBL statements CBL |
                                    DISPLAY CLN displayleftfact |
                                    DISPLAYLINE CLN displaylineleftfact |
                                    DQSTR CLN READ CLN VAR SCLN |
                                    CMT |
                                    RETURN CLN exp SCLN comment
*/

// comment                      --> CMT | ^

// relationaloperator           --> EQL | NEQ | LTN | LTE | GTR | GTE

// ifconditionleftfact          --> ELSE elsecondtionleftfact | ^

// elsecondtitionleftfact       --> IF OPTS exp relationaloperator exp CPTS THEN SBL statement CBL ifconditionleftfact |
//                                  SBL statement CBL

// varasnleftfact               --> CLN vartype asnleftfact |
//                                  ASN exp SCLN

// asnleftfact                  --> ASN ansvalueleftfact |
//                                  SCLN

// asnvalueleftfact             --> EXECUTE CLN ID OPTS expoccurences CPTS SCLN | exp SCLN

// expoccurences                --> expoccurence expoccurences | ^

// expoccurence                 --> exp multiexpcomma

// multiexpcomma                --> CMA expoccurence | ^

// vartype                      --> INT | CHAR | FLOAT

// displayleftfact              --> DQSTR SCLN |
//                                  SQSTR SCLN |
//                                  VAR CMA displaylineleftfact

// displaylineleftfact          --> DQSTR SCLN |
//                                  SQSTR SCLN

// exp                          --> lowpre exp1

// exp1                         --> ADD lowpre exp1 | SBT lowpre exp1 | ^

// lowpre                       --> highpre lowpre1

// lowpre1                      --> MUL highpre lowpre1 | DIV highpre lowpre1 | MOD highpre lowpre1 | ^

// highpre                      --> VAR | NUM | OPTS exp CPTS

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// CFG LL(1) Implementation //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// functions                --> function functions | ^
void parser::functions()
{
    if (_lexer.peek(1).tokenType == TokenType::ID ||
        _lexer.peek(1).tokenType == TokenType::VAR ||
        _lexer.peek(1).tokenType == TokenType::CMT)
    {
        function();
        functions();
    }
    else
    {
        ;
    }
}
// function                 --> ID : vartype <- FUNCTION {function.Name = id.lex ,function.line = lineNo)} ( params ) { statements }
//                              VAR : vartype gvarasnleftfact
//                              CMT
void parser::function()
{
    if (_lexer.peek(1).tokenType == TokenType::ID)
    {
        string v = expect(TokenType::ID).lexeme;
        cout << "function(ID)\n";
        expect(TokenType::CLN);
        cout << "function(:)\n";

        vartype();

        expect(TokenType::FPASN);
        cout << "function(<-)\n";
        expect(TokenType::FUNCTION);
        cout << "function(FUNCTION)\n";

        // Saving Every Function with it's relevant line no and it's name
        funcDeclaration tmp;
        tmp.funcName = v;
        tmp.lineNo = lineNo;
        parser::func.push_back(tmp);

        expect(TokenType::OPTS);
        cout << "function(()\n";

        params();

        expect(TokenType::CPTS);
        cout << "function())\n";
        expect(TokenType::SBL);
        cout << "function({)\n";

        statements();

        expect(TokenType::CBL);
        cout << "function(})\n";
    }
    else if (_lexer.peek(1).tokenType == TokenType::VAR)
    {
        ////////////////////////////////// Symbols for Parameter Variable //////////////////////////////////
        if (_lexer.peek(1).tokenType == TokenType::VAR)
        {
            if (_lexer.peek(2).tokenType == TokenType::CLN)
            {
                if (_lexer.peek(3).tokenType == TokenType::INT ||
                    _lexer.peek(3).tokenType == TokenType::CHAR ||
                    _lexer.peek(3).tokenType == TokenType::FLOAT)
                {
                    if (!symboltable.is_open())
                    {
                        symboltable.open("symbol_table.txt", fstream::app);
                    }
                    int space = 45 - _lexer.peek(1).lexeme.size();
                    symboltable << _lexer.peek(1).lexeme;
                    symboltable << setw(space);

                    if (_lexer.peek(3).tokenType == TokenType::INT)
                    {
                        symboltable << "INT"
                                    << "\n";
                    }
                    else if (_lexer.peek(3).tokenType == TokenType::CHAR)
                    {
                        symboltable << "CHAR"
                                    << "\n";
                    }
                    else
                    {
                        symboltable << "FLOAT"
                                    << "\n";
                    }
                    symboltable << "--------------------------------------------------\n";
                    symboltable.close();
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        expect(TokenType::VAR);
        cout << "function(VAR)\n";
        expect(TokenType::CLN);
        cout << "function(:)\n";

        vartype();

        gvarasnleftfact();
    }
    else if (_lexer.peek(1).tokenType == TokenType::CMT)
    {
        expect(TokenType::CMT);
        cout << "function(#~ ~#)\n";
    }
    else
    {
        syntax_error();
    }
}
// gvarasnleftfact         --> = exp ; | ;
void parser::gvarasnleftfact()
{
    if (_lexer.peek(1).tokenType == TokenType::ASN)
    {
        expect(TokenType::ASN);
        cout << "gvarasnleftfact(=)\n";

        exp();

        expect(TokenType::SCLN);
        cout << "gvarasnleftfact(;)\n";
    }
    else if (_lexer.peek(1).tokenType == TokenType::SCLN)
    {
        expect(TokenType::SCLN);
        cout << "gvarasnleftfact(;)\n";
    }
    else
    {
        syntax_error();
    }
}
// params                   --> param params | ^
void parser::params()
{
    if (_lexer.peek(1).tokenType == TokenType::VAR)
    {
        param();
        params();
    }
    else
    {
        ;
    }
}
// param                   --> VAR : vartype multiparamcomma
void parser::param()
{
    if (_lexer.peek(1).tokenType == TokenType::VAR)
    {
        ////////////////////////////////// Symbols for Parameter Variable //////////////////////////////////
        if (_lexer.peek(1).tokenType == TokenType::VAR)
        {
            string parinit = "param";
            if (_lexer.peek(2).tokenType == TokenType::CLN)
            {
                if (_lexer.peek(3).tokenType == TokenType::INT ||
                    _lexer.peek(3).tokenType == TokenType::CHAR ||
                    _lexer.peek(3).tokenType == TokenType::FLOAT)
                {
                    if (!symboltable.is_open())
                    {
                        symboltable.open("symbol_table.txt", fstream::app);
                    }
                    int space = 45 - _lexer.peek(1).lexeme.size();
                    symboltable << _lexer.peek(1).lexeme;
                    symboltable << setw(space);
                    space = 42 - parinit.size();
                    symboltable << parinit;
                    symboltable << setw(space);

                    if (_lexer.peek(3).tokenType == TokenType::INT)
                    {
                        symboltable << "INT";
                        symboltable <<"                                         "<<varAddress;
                        varAddress = varAddress + 4;
                        symboltable << "\n";
                    }
                    else if (_lexer.peek(3).tokenType == TokenType::CHAR)
                    {
                        symboltable << "CHAR";
                        symboltable <<"                                         "<< varAddress;
                        varAddress = varAddress + 1;
                        symboltable << "\n";
                    }
                    else
                    {
                        symboltable << "FLOAT"
                                    << "\n";
                    }
                    symboltable << "------------------------------------------------------------------------------------------------------------------------------------------------\n";
                    symboltable.close();
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        expect(TokenType::VAR);
        cout << "param(VAR)\n";
        expect(TokenType::CLN);
        cout << "param(:)\n";

        vartype();

        multiparamcomma();
    }
    else
    {
        syntax_error();
    }
}
// multiparamcomma         --> , param | ^
void parser::multiparamcomma()
{
    if (_lexer.peek(1).tokenType == TokenType::CMA)
    {
        expect(TokenType::CMA);
        cout << "multiparamcomma(,)\n";

        param();
    }
    else
    {
        ;
    }
}
// statements               --> statement statements | ^
void parser::statements()
{
    if (_lexer.peek(1).tokenType == TokenType::IF ||
        _lexer.peek(1).tokenType == TokenType::VAR ||
        _lexer.peek(1).tokenType == TokenType::DO ||
        _lexer.peek(1).tokenType == TokenType::DISPLAY ||
        _lexer.peek(1).tokenType == TokenType::DISPLAYLINE ||
        _lexer.peek(1).tokenType == TokenType::DQSTR ||
        _lexer.peek(1).tokenType == TokenType::CMT ||
        _lexer.peek(1).tokenType == TokenType::RETURN)
    {
        statement();
        statements();
    }
    else
    {
        ;
    }
}
/*
 statement                 --> IF ( exp relationaloperator exp ) { statement.ifStart = lineNO
//                                                                 emit("if " + id.lex + " " + ro + " " + id.lex + " goto")
//                                                                 statement.elseStart = lineNO
//                                                                 emit("goto") } THEN {backPatch(statement.ifStart,to_string(lineNo))}
//                                                                 { statements } {ifconditionleftfact.s = statement.elseStart} ifconditionleftfact
                               VAR varasnleftfact {statement.s = varasnleftfact.s}
                               DO UNTIL ( exp relationaloperator exp ) { statement.ifStart = lineNO
//                                                                       emit("if " + id.lex + " " + ro + " " + id.lex + " goto")
//                                                                       statement.elseStart = lineNO
//                                                                       emit("goto")
//                                                                       backPatch(statement.ifStart,to_string(lineNo))
//                                                                       statement.loopIf = lineNo;
//                                                                       emit("goto");
//                                                                       backPatch(loopIf,to_string(ifStart));
//                                                                       backPatch(elseStart,to_string(lineNo))} { statements }
//                             DISPLAY : displayleftfact {statement.s = displayleftfact.s}
//                             DISPLAYLINE : displaylineleftfact {statement.s = displaylineleftfact.s}
//                             DQSTR : READ : VAR ; { emit("out " + DQSTR.lex)
//                                                    emit("in " + VAR.lex)  }
                               CMT
                               RETURN : exp ; comment {statement.ret = exp.s}
*/
void parser::statement()
{
    if (_lexer.peek(1).tokenType == TokenType::IF)
    {
        expect(TokenType::IF);
        cout << "statement(IF)\n";
        expect(TokenType::OPTS);
        cout << "statement(()\n";

        string id1 = exp();

        string ro = relationaloperator();

        string id2 = exp();

        int ifStart = lineNo;
        emit("if " + id1 + " " + ro + " " + id2 + " goto");
        int elseStart = lineNo;
        emit("goto");

        expect(TokenType::CPTS);
        cout << "statement())\n";
        expect(TokenType::THEN);
        cout << "statement(THEN)\n";

        backPatch(ifStart, to_string(lineNo));

        expect(TokenType::SBL);
        cout << "statement({)\n";

        statements();

        expect(TokenType::CBL);
        cout << "statement(})\n";

        ifconditionleftfact(elseStart);
    }
    else if (_lexer.peek(1).tokenType == TokenType::VAR)
    {
        ////////////////////////////////// Symbols for Parameter Variable //////////////////////////////////
        if (_lexer.peek(1).tokenType == TokenType::VAR)
        {
            string init = "0";
            string ninit = "null";
            string nfinit = "function ret";
            if (_lexer.peek(2).tokenType == TokenType::CLN)
            {
                if (_lexer.peek(3).tokenType == TokenType::INT ||
                    _lexer.peek(3).tokenType == TokenType::CHAR ||
                    _lexer.peek(3).tokenType == TokenType::FLOAT)
                {
                    if (!symboltable.is_open())
                    {
                        symboltable.open("symbol_table.txt", fstream::app);
                    }
                    int space = 45 - _lexer.peek(1).lexeme.size();
                    symboltable << _lexer.peek(1).lexeme;//Var Name
                    symboltable << setw(space);

                    if(_lexer.peek(4).tokenType == TokenType::ASN)
                    {
                        if(_lexer.peek(5).tokenType == TokenType::NUM)
                        {
                            int space = 38 - _lexer.peek(5).lexeme.size();
                            symboltable<<_lexer.peek(5).lexeme;
                            symboltable << setw(space);
                        }
                        else
                        {
                             int space = 37 - _lexer.peek(7).lexeme.size();
                            symboltable<<"call "<<_lexer.peek(7).lexeme;
                            symboltable << setw(space);
                        }
                    }
                    else
                    {
                        int space = 41 - ninit.size();
                        symboltable<<ninit;
                        symboltable << setw(space);
                    }

                    if (_lexer.peek(3).tokenType == TokenType::INT)
                    {
                        symboltable << "INT";
                        symboltable <<"                                         "<< varAddress;
                        varAddress = varAddress +4;
                        symboltable << "\n";
                    }
                    else if (_lexer.peek(3).tokenType == TokenType::CHAR)
                    {
                        symboltable << "CHAR";
                        symboltable <<"                                         "<< varAddress;
                        varAddress = varAddress + 1;
                         symboltable << "\n";
                    }
                    else
                    {
                        symboltable << "FLOAT"
                                    << "\n";
                    }
                    symboltable << "------------------------------------------------------------------------------------------------------------------------------------------------\n";
                    symboltable.close();
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        string var = simName(expect(TokenType::VAR).lexeme);
        cout << "statement(VAR)\n";

        string v = varasnleftfact();
        //Not to emit non initialized variables
        if (v != "")
        {
            emit(var + " = " + v);
        }
    }
    else if (_lexer.peek(1).tokenType == TokenType::DO)
    {
        expect(TokenType::DO);
        cout << "statement(DO)\n";
        expect(TokenType::UNTIL);
        cout << "statement(UNTIL)\n";
        expect(TokenType::OPTS);
        cout << "statement(()\n";

        string id1 = exp();

        string ro = relationaloperator();

        string id2 = exp();

        int ifStart = lineNo;
        emit("if " + id1 + " " + ro + " " + id2 + " goto");
        int elseStart = lineNo;
        emit("goto");

        expect(TokenType::CPTS);
        cout << "statement())\n";

        backPatch(ifStart, to_string(lineNo));

        expect(TokenType::SBL);
        cout << "statement({)\n";

        statements();

        expect(TokenType::CBL);
        cout << "statement(})\n";

        int loopIf = lineNo;
        emit("goto");
        backPatch(loopIf, to_string(ifStart));
        backPatch(elseStart, to_string(lineNo));
    }
    else if (_lexer.peek(1).tokenType == TokenType::DISPLAY)
    {
        expect(TokenType::DISPLAY);
        cout << "statement(DISPLAY)\n";
        expect(TokenType::CLN);
        cout << "statement(:)\n";

        string v = displayleftfact();
        emit("out " + v);
    }
    else if (_lexer.peek(1).tokenType == TokenType::DISPLAYLINE)
    {
        expect(TokenType::DISPLAYLINE);
        cout << "statement(DISPLAYLINE)\n";
        expect(TokenType::CLN);
        cout << "statement(:)\n";
        string v = displaylineleftfact();
        emit("out " + v);
    }
    else if (_lexer.peek(1).tokenType == TokenType::RETURN)
    {
        expect(TokenType::RETURN);
        cout << "statement(RETURN)\n";
        expect(TokenType::CLN);
        cout << "statement(:)\n";

        string ret = exp();
        emit("ret " + ret);

        expect(TokenType::SCLN);
        cout << "statement(;)\n";

        comment();
    }
    else if (_lexer.peek(1).tokenType == TokenType::CMT)
    {
        expect(TokenType::CMT);
        cout << "statement(#~ ~#)\n";
    }
    else if (_lexer.peek(1).tokenType == TokenType::DQSTR)
    {
        string readIns = expect(TokenType::DQSTR).lexeme;
        cout << "statement(DQSRT)\n";
        emit("out " + readIns);

        expect(TokenType::CLN);
        cout << "statement(:)\n";
        expect(TokenType::READ);
        cout << "statement(READ)\n";
        expect(TokenType::CLN);
        cout << "statement(:)\n";

        string readVar = simName(expect(TokenType::VAR).lexeme);
        cout << "statement(VAR)\n";
        emit("in " + readVar);

        expect(TokenType::SCLN);
        cout << "statement(;)\n";
    }
    else
    {
        syntax_error();
    }
}
// comment                 --> CMT | ^
void parser::comment()
{
    if (_lexer.peek(1).tokenType == TokenType::CMT)
    {
        expect(TokenType::CMT);
        cout << "statement(#~ ~#)\n";
    }
    else
    {
        ;
    }
}
// relationaloperator      --> EQL | NEQ | LTN | LTE | GTR | GTE
string parser::relationaloperator()
{
    if (_lexer.peek(1).tokenType == TokenType::EQL)
    {
        string v = expect(TokenType::EQL).lexeme;
        cout << "relationaloperator(-eq)\n";
        return "==";
    }
    else if (_lexer.peek(1).tokenType == TokenType::NEQ)
    {
        string v = expect(TokenType::NEQ).lexeme;
        cout << "relationaloperator(-ne)\n";
        return "!=";
    }
    else if (_lexer.peek(1).tokenType == TokenType::LTN)
    {
        string v = expect(TokenType::LTN).lexeme;
        cout << "relationaloperator(-lt)\n";
        return "<";
    }
    else if (_lexer.peek(1).tokenType == TokenType::LTE)
    {
        string v = expect(TokenType::LTE).lexeme;
        cout << "relationaloperator(-le)\n";
        return "<=";
    }
    else if (_lexer.peek(1).tokenType == TokenType::GTR)
    {
        string v = expect(TokenType::GTR).lexeme;
        cout << "relationaloperator(-gt)\n";
        return ">";
    }
    else if (_lexer.peek(1).tokenType == TokenType::GTE)
    {
        string v = expect(TokenType::GTE).lexeme;
        cout << "relationaloperator(-ge)\n";
        return ">=";
    }
    else
    {
        syntax_error();
        return "";
    }
}
// ifconditionleftfact     --> ELSE elsecondtionleftfact { ifconditionleftfact.elseSkip = lineNo
//                                                         emit("goto")
//                                                         backPatch(ifconditionleftfact.elseSkip,to_string(lineNo))}
//                             ^
void parser::ifconditionleftfact(int elseStart)
{
    if (_lexer.peek(1).tokenType == TokenType::ELSE)
    {

        int elseSkip = lineNo;
        emit("goto");

        backPatch(elseStart, to_string(lineNo));

        expect(TokenType::ELSE);
        cout << "ifconditionleftfact(ELSE)\n";

        elseconditionleftfact();

        backPatch(elseSkip, to_string(lineNo));
    }
    else
    {
        ;
    }
}
// elsecondtitionleftfact  --> IF ( exp relationaloperator exp ) { elsecondtitionleftfact.ifStart = lineNO
//                                                                 emit("if " + id.lex + " " + ro + " " + id.lex + " goto")
//                                                                 elsecondtitionleftfact.elseStart = lineNO
//                                                                 emit("goto") } THEN {backPatch(elsecondtitionleftfactifStart,to_string(lineNo))}
//                                                                 { statements } {ifconditionleftfact.s = elsecondtitionleftfact.elseStart} ifconditionleftfact
//                             { statements }
void parser::elseconditionleftfact()
{
    if (_lexer.peek(1).tokenType == TokenType::IF)
    {
        expect(TokenType::IF);
        cout << "elseconditionleftfact(IF)\n";
        expect(TokenType::OPTS);
        cout << "elseconditionleftfact(()\n";

        string id1 = exp();

        string ro = relationaloperator();

        string id2 = exp();

        int ifStart = lineNo;
        emit("if " + id1 + " " + ro + " " + id2 + " goto");
        int elseStart = lineNo;
        emit("goto");

        expect(TokenType::CPTS);
        cout << "elseconditionleftfact())\n";
        expect(TokenType::THEN);
        cout << "elseconditionleftfact(THEN)\n";

        backPatch(ifStart, to_string(lineNo));

        expect(TokenType::SBL);
        cout << "elseconditionleftfact({)\n";

        statements();

        expect(TokenType::CBL);
        cout << "elseconditionleftfact(})\n";

        ifconditionleftfact(elseStart);
    }
    else if (_lexer.peek(1).tokenType == TokenType::SBL)
    {
        expect(TokenType::SBL);
        cout << "elseconditionleftfact({)\n";

        statements();

        expect(TokenType::CBL);
        cout << "elseconditionleftfact(})\n";
    }
    else
    {
        syntax_error();
    }
}
// varasnleftfact          --> : vartype  asnleftfact {varasnleftfact.s = asnleftfact.s}
//                             = exp ; {varasnleftfact.s = exp.s}
string parser::varasnleftfact()
{
    if (_lexer.peek(1).tokenType == TokenType::CLN)
    {
        expect(TokenType::CLN);
        cout << "varasnleftfact(:)\n";

        vartype();

        string v = asnleftfact();
        return v;
    }
    else if (_lexer.peek(1).tokenType == TokenType::ASN)
    {
        expect(TokenType::ASN);
        cout << "varasnleftfact(=)\n";

        string v = exp();

        expect(TokenType::SCLN);
        cout << "varasnleftfact(;)\n";
        return v;
    }
}
// asnleftfact             --> = asnvalueleftfact {asnleftfact.s = asnvalueleftfact.s}
//                             ;
string parser::asnleftfact()
{
    if (_lexer.peek(1).tokenType == TokenType::ASN)
    {
        expect(TokenType::ASN);
        cout << "asnleftfact(=)\n";

        string v = asnvalueleftfact();
        return v;
    }
    else if (_lexer.peek(1).tokenType == TokenType::SCLN)
    {
        expect(TokenType::SCLN);
        cout << "asnleftfact(;)\n";
        return "";
    }
}
// asnvalueleftfact        --> EXECUTE : ID ( expoccurences ) ; {tmp = tempGen()
//                                                               emit("call " + id.lex + id.finfFunc(id.lex) + " , " + tmp)
//                                                               asnvalueleftfact.v = tmp}
//                             exp ;                            {asnvalueleftfact.v = exp.v}
string parser::asnvalueleftfact()
{
    if (_lexer.peek(1).tokenType == TokenType::EXECUTE)
    {
        expect(TokenType::EXECUTE);
        cout << "asnvalueleftfact(EXECUTE)\n";
        expect(TokenType::CLN);
        cout << "asnvalueleftfact(:)\n";

        string v = expect(TokenType::ID).lexeme;
        cout << "asnvalueleftfact(ID)\n";

        int funDecLine = parser::findFunc(v);

        expect(TokenType::OPTS);
        cout << "asnvalueleftfact(()\n";

        expoccurences();

        expect(TokenType::CPTS);
        cout << "asnvalueleftfact())\n";
        expect(TokenType::SCLN);
        cout << "asnvalueleftfact(;)\n";

        string t = tempGen();
        emit("call " + v + " " + to_string(funDecLine) + " , " + t);
        return t;
    }
    else if (_lexer.peek(1).tokenType == TokenType::VAR || _lexer.peek(1).tokenType == TokenType::NUM)
    {
        string v = exp();

        expect(TokenType::SCLN);
        cout << "asnvalueleftfact(;)\n";
        return v;
    }
    else
    {
        syntax_error();
        return "";
    }
}
// expoccurences           --> expoccurence expoccurences | ^
void parser::expoccurences()
{
    if (_lexer.peek(1).tokenType == TokenType::VAR || _lexer.peek(1).tokenType == TokenType::NUM)
    {
        expoccurence();
        expoccurences();
    }
    else
    {
        ;
    }
}
// expoccurence            --> exp multiexpcomma {emit("param " + exp.s + multiexpcomma.s)}
void parser::expoccurence()
{
    if (_lexer.peek(1).tokenType == TokenType::VAR || _lexer.peek(1).tokenType == TokenType::NUM)
    {
        cout << "expoccurence(exp)\n";

        string v = exp();
        emit("param " + v);

        multiexpcomma();
    }
    else
    {
        syntax_error();
    }
}
// multiexpcomma           --> , expoccurence
//                               ^
void parser::multiexpcomma()
{
    if (_lexer.peek(1).tokenType == TokenType::CMA)
    {
        expect(TokenType::CMA);
        cout << "multiexpcomma(,)\n";

        expoccurence();
    }
    else
    {
        ;
    }
}
// vartype                 --> INT | CHAR | FLOAT
void parser::vartype()
{
    if (_lexer.peek(1).tokenType == TokenType::INT)
    {
        expect(TokenType::INT);
        cout << "vartype(INT)\n";
    }
    else if (_lexer.peek(1).tokenType == TokenType::CHAR)
    {
        expect(TokenType::CHAR);
        cout << "vartype(CHAR)\n";
    }
    else if (_lexer.peek(1).tokenType == TokenType::FLOAT)
    {
        expect(TokenType::FLOAT);
        cout << "vartype(FLOAT)\n";
    }
    else
    {
        syntax_error();
    }
}
// displayleftfact         --> " " ; {displayleftfact.s = DQSTR.lex} //DQSTR = Double Quoted String
//                             ' ' ; {displayleftfact.s = SQSTR.lex} //SQSTR = Single Quoted String
//                             VAR , " " ; {emit("out " + VAR.lex);
//                                          displayleftfact.s = displaylineleftfact.s}
string parser::displayleftfact()
{
    if (_lexer.peek(1).tokenType == TokenType::DQSTR)
    {
        string v = expect(TokenType::DQSTR).lexeme;
        cout << "displayleftfact(DQSTR)\n";
        expect(TokenType::SCLN);
        cout << "displayleftfact(;)\n";
        return v;
    }
    else if (_lexer.peek(1).tokenType == TokenType::SQSTR)
    {
        string v = expect(TokenType::SQSTR).lexeme;
        cout << "displayleftfact(SQSTR)\n";
        expect(TokenType::SCLN);
        cout << "displayleftfact(;)\n";
        return v;
    }
    else if (_lexer.peek(1).tokenType == TokenType::VAR)
    {
        string v = simName(expect(TokenType::VAR).lexeme);
        emit("out " + v);
        cout << "displayleftfact(VAR)\n";
        expect(TokenType::CMA);
        cout << "displayleftfact(,)\n";
        string v2 = displaylineleftfact();
        return v2;
    }
    else
    {
        syntax_error();
        return "";
    }
}
// displaylineleftfact     --> " " ; {displaylineleftfact.s = DQSTR.lex}
//                             ' ' ; {displaylineleftfact.s = SQSTR.lex}
string parser::displaylineleftfact()
{
    if (_lexer.peek(1).tokenType == TokenType::DQSTR)
    {
        string v = expect(TokenType::DQSTR).lexeme;
        cout << "displaylineleftfact(DQSTR)\n";
        expect(TokenType::SCLN);
        cout << "displaylineleftfact(;)\n";
        return v;
    }
    else if (_lexer.peek(1).tokenType == TokenType::SQSTR)
    {
        string v = expect(TokenType::SQSTR).lexeme;
        cout << "displaylineleftfact(SQSTR)\n";
        expect(TokenType::SCLN);
        cout << "displaylineleftfact(;)\n";
        return v;
    }
    else
    {
        syntax_error();
        return "";
    }
}
// exp                        --> lowpre    {exp1.i = lowpre.v}
//                                exp1      {exp.v = exp1.v}
string parser::exp()
{
    cout << "exp()\n";
    string s = lowpre() + exp1();
    return s;
}
// exp1                       --> + lowpre exp1 {exp1.s = " + " + lowpre.s}
//                                - lowpre exp1 {exp1.s = " - " + lowpre.s}
//                                  ^ {exp1.s = ""} //null string
string parser::exp1()
{
    if (_lexer.peek(1).tokenType == TokenType::ADD)
    {
        expect(TokenType::ADD);
        cout << "exp1(+)\n";

        string s = lowpre();
        exp1();
        return " + " + s;
    }
    else if (_lexer.peek(1).tokenType == TokenType::SBT)
    {
        expect(TokenType::SBT);
        cout << "exp1(-)\n";

        string s = lowpre();
        exp1();
        return " - " + s;
    }
    else
    {
        return "";
    }
}
// lowpre                     --> highpre lowpre1 {lowpre.v = highpre.s + lowpre1.s}
string parser::lowpre()
{
    string s = highpre() + lowpre1();
    return s;
}
// lowpre1                    --> * highpre lowpre1 {lowpre1.s = " * " + highpre.s}
//                                / highpre lowpre1 {lowpre1.s = " / " + highpre.s}
//                                % highpre lowpre1 {lowpre1.s = " % " + highpre.s}
//                                  ^ {exp1.s = ""} //null string
string parser::lowpre1()
{
    if (_lexer.peek(1).tokenType == TokenType::MUL)
    {
        expect(TokenType::MUL);
        cout << "lowpre1(*)\n";

        string s = highpre();
        lowpre1();
        return " * " + s;
    }
    else if (_lexer.peek(1).tokenType == TokenType::DIV)
    {
        expect(TokenType::DIV);
        cout << "lowpre1(/)\n";

        string s = highpre();
        lowpre1();
        return " / " + s;
    }
    else if (_lexer.peek(1).tokenType == TokenType::MOD)
    {
        expect(TokenType::MOD);
        cout << "lowpre1(%)\n";

        string s = highpre();
        lowpre1();
        return " % " + s;
    }
    else
    {
        return "";
    }
}
// highpre                    --> VAR {highpre.s = VAR.lex}
//                                NUM {highpre.s = NUM.lex}
//                               ( exp ) {highpre.s = exp.s}
string parser::highpre()
{
    if (_lexer.peek(1).tokenType == TokenType::VAR)
    {
        cout << "highpre(VAR)\n";
        // simName Method takes a string and removes $ sign from TAC and returns the simplified string
        string s = simName(expect(TokenType::VAR).lexeme);
        return s;
    }
    else if (_lexer.peek(1).tokenType == TokenType::NUM)
    {
        cout << "highpre(NUM)\n";
        return expect(TokenType::NUM).lexeme;
    }
    else if (_lexer.peek(1).tokenType == TokenType::OPTS)
    {
        cout << "highpre(()\n";
        expect(TokenType::OPTS);

        string s = exp();
        cout << "highpre())\n";
        expect(TokenType::CPTS);
        return s;
    }
    else
    {
        syntax_error();
        return "";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////-Alternative Possible Translation Scheme for Expression-////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// exp                           --> lowpre { exp1.i = lowpre.v }
//                                    exp1 { exp.v = exp1.v }
// string parser::exp()
// {
//     cout << "exp()\n";
//     string v = lowpre();

//     return exp1(v);
// }

// exp1                          -->  ADD lowpre { t = newTemp(); 
//                                                 emit(t," = ", exp.i, " + ", lowpre.v);
//                                                 exp1.i = t }
//                                    exp1 { exp.v = exp1.v }
// exp1	                         -->  SBT lowpre { t = newTemp(); 
//                                                 emit(t," = ", exp.i, " - ", lowpre.v);
//                                                 exp1.i = t }
// exp1                               exp1 { exp.v = exp1.v }
// exp1	                              ^ { exp.v = exp.i }
// string parser::exp1(string i)
// {
//     if (_lexer.peek(1).tokenType == TokenType::ADD)
//     {
//         expect(TokenType::ADD);
//         cout << "exp1(+)\n";

//         string v = lowpre();
//         string t = tempGen();
//         emit(t + " = " + i + " + " + v);

//         return exp1(t);
//     }
//     else if (_lexer.peek(1).tokenType == TokenType::SBT)
//     {
//         expect(TokenType::SBT);
//         cout << "exp1(-)\n";

//         string v = lowpre();
//         string t = tempGen();
//         emit(t + " = " + i + " - " + v);

//         return exp1(t);
//     }
//     return i;
// }

// lowpre                        -->  highpre { highpre.i = lowpre1.v }
//                                    lowpre1 { lowpre.v = highpre.v }
// string parser::lowpre()
// {
//     string v = highpre();

//     return lowpre1(v);
// }

// lowpre1	                     -->  MUL highpre { t = newTemp();
//                                                  emit(t," = ", highpre.i, " * ", lowpre1.v);
//                                                  lowpre1.i = t }
//                                    lowpre1 { highpre.v = highpre(1).v }
// lowpre1	                     -->  DIV highpre { t = newTemp();
//                                                  emit(t," = ", highpre.i, " / ", lowpre1.v);
//                                                  lowpre1.i = t }
//                                    lowpre1  { highpre.v = highpre(1).v }
// lowpre1	                     -->  MOD highpre { t = newTemp();
//                                                  emit(t," = ", highpre.i, " % ", lowpre1.v);
//                                                  lowpre1.i = t }
//                                    lowpre1  {highpre.v = highpre(1).v }
// lowpre1	                          ^ { highpre.v = highpre.i }
// string parser::lowpre1(string i)
// {
//     if (_lexer.peek(1).tokenType == TokenType::MUL)
//     {
//         expect(TokenType::MUL);
//         cout << "lowpre1(*)\n";

//         string v = highpre();
//         string t = tempGen();
//         emit(t + " = " + i + " * " + v);
//         return lowpre1(t);
//     }
//     else if (_lexer.peek(1).tokenType == TokenType::DIV)
//     {
//         expect(TokenType::DIV);
//         cout << "lowpre1(/)\n";

//         string v = highpre();
//         string t = tempGen();
//         emit(t + " = " + i + " / " + v);
//         return lowpre1(t);
//     }
//     else if (_lexer.peek(1).tokenType == TokenType::MOD)
//     {
//         expect(TokenType::MOD);
//         cout << "lowpre1(%)\n";

//         string v = highpre();
//         string t = tempGen();
//         emit(t + " = " + i + " % " + v);
//         return lowpre1(t);
//     }
//     return i;    
// }

// highpre	                     -->  ID {highpre.v = ID.lex}
// highpre                            Num {highpre.v = toInt(NUM.lex)} 
// highpre                            ( exp )
// string parser::highpre()
// {
//     if (_lexer.peek(1).tokenType == TokenType::VAR)
//     {
//         cout << "highpre(VAR)\n";
//         return expect(TokenType::VAR).lexeme;
//     }
//     else if (_lexer.peek(1).tokenType == TokenType::NUM)
//     {
//         cout << "highpre(NUM)\n";
//         return expect(TokenType::NUM).lexeme;
//     }
//     else if (_lexer.peek(1).tokenType == TokenType::OPTS)
//     {
//         cout << "highpre(()\n";
//         expect(TokenType::OPTS);

//         string v = exp();
//         cout << "highpre())\n";
//         expect(TokenType::CPTS);
//         return v;
//     }
//     else
//     {
//         syntax_error();
//         return " ";
//     }
// }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////-Alternative Possible Translation Scheme for Expression-////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Temp variable name generator
string parser::tempGen()
{
    string temp = "t" + std::to_string(tempCall++);
    return temp;
}

// Emit TAC
void parser::emit(string tacInstruction)
{
    tac.push_back(tacInstruction);
    lineNo++;
}

// Save TAC vector to a file
void parser::saveTAC()
{
    if (!tacSave.is_open())
    {
        tacSave.open("TAC.txt");
    }
    for (int i = 0; i < tac.size(); i++)
    {
        tacSave << i + 1 << ":\t" << tac[i] << ";";
        if (i < tac.size() - 1)
        {
            tacSave << "\n";
        }
    }
    tacSave.close();
}

// FillingIn the Blanks
void parser::backPatch(int lNo, string pVal)
{
    tac[lNo - 1] = tac[lNo - 1] + " " + pVal;
}

// Removing '$' FROM Vars and IDs
string parser::simName(string tmp)
{
    tmp.erase(remove(tmp.begin(), tmp.end(), '$'), tmp.begin());
    return tmp;
}

//Stores All the functions with their lineNo relevence
int parser::findFunc(string v)
{
    for (int i = 0; i < parser::func.size(); i++)
    {
        funcDeclaration tmp = parser::func[i];
        if (tmp.funcName == v)
        {
            return tmp.lineNo;
        }
    }
}
