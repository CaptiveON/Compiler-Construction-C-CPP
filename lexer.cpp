#include "lexer.h"
#include <regex>
using namespace std;
// for printing tokens names, Same as the enum defined in lexer.h
string reserved[] = {
    "END_OF_FILE",
    "ERROR",
    "FUNCTION",
    "INT",
    "CHAR",
    "FLOAT",
    "IF",
    "ELSE",
    "DO",
    "UNTIL",
    "THEN",
    "READ",
    "DISPLAY",
    "DISPLAYLINE",
    "RETURN",
    "EXECUTE",
    "NUM",
    "ID",
    "VAR",
    "CMA",   // COMMA
    "CLN",   // COLON
    "SCLN",  // SEMI COLON
    "DLR",   // DOLLAR
    "OPTS",  // OPENING PARENTHESIS
    "CPTS",  // CLOSING PARENTHESIS
    "ADD",   // ADD
    "SBT",   // SUBTRACT
    "MUL",   // MULTIPLY
    "DIV",   // DIVIDE
    "MOD",   // MODULUS
    "SBL",   // START OF BLOCK
    "CBL",   // END OF BLOCK
    "ASN",   // ASSIGNMENT
    "FPASN", // FUNCTION POINTER ASSIGNMENT OPERATOR
    "EQL",   // EQUAL TO
    "NEQ",   // NOT EQUAL TO
    "LTN",   // LESS THAN
    "LTE",   // LESS THAN EQUAL TO
    "GTR",   // GREATER THAN
    "GTE",   // GREATER THAN EQUAL TO
    "CMT",   // COMMENT
    "DQSTR", // DOUBLE QUOTATION STRING
    "SQSTR"  // SINGLE QUOTATION STRING
};
token::token()
{
    this->lexeme = "";
    this->tokenType = TokenType::ERROR;
}
token::token(int lineNo,string lexeme, TokenType tokenType)
{
    this->lineNo = lineNo;
    this->lexeme = lexeme;
    this->tokenType = tokenType;
}
void token::Print()
{
    cout << "{" <<lineNo<<" , "<< lexeme << " , "
         << reserved[(int)tokenType] << "}\n";
}
int lexer::getCurrentPointer()
{
    return index;
}
void lexer::setCurrentPointer(int pos)
{
    if (pos >= 0 && pos < tokens.size())
        index = pos;
    else
        index = 0;
}
void lexer::Tokenize() // function that tokenizes your input stream
{
    // Iterating data stored in lexer that was read from the File
    vector<char>::iterator it = stream.begin();
    /* RegularExpression Library made for each major distinguished case
    such as Identifiers, Keywords, Variables and Numerics. But all the cases
    are overlapping in nature i-e Keywords, Identifiers, and Variables,
    all contain Alphabets so "superWord regex" categorizes all on behalf of
    their mutual nature. It distinguishes Keywords, Variables, and Identifiers
    while identifying there distinguished rules such as Symbols($,_) or the
    combination of Symbols. */
    regex superWord("\\$+|_+|[a-zA-Z0-9]+|\\$_[a-zA-Z0-9]*|\\$[a-zA-Z0-9]*|_[a-zA-Z0-9]*");
    regex keyWord("function|int|char|float|if|else|do|until|then|read|display|displayline|return|execute");
    regex identifier("_[a-zA-Z][a-zA-Z0-9]*|[a-zA-Z][a-zA-Z0-9]*");
    regex variable("\\$|\\$_[a-zA-Z][a-zA-Z0-9]*|\\$[a-zA-Z][a-zA-Z0-9]*");
    regex numeric("[0-9]+");
    // Managing states for each different category of occurences.
    int state = 0;
    int lineNo = 1;
    // All the checking will be performed on this string.
    // It will always be at it(iterator)-1 index as compared to it.
    string lexeme;
    lexeme.push_back(*it++);
    while (it < stream.end())
    {
        switch (state)
        {
        // It is the starting case and each time lexeme enters in case 0,
        // it will posses only one character
        case 0:
            // Found Space(" ")
            if (lexeme == " ")
            {
                lexeme.clear();
                lexeme.push_back(*it++);
            }
            // Found KEYWORD|IDENTIFIER|NUMERIC|VARIABLE
            else if (regex_match(lexeme, superWord))
            {
                while (regex_match(lexeme, superWord))
                {
                    lexeme.push_back(*it++);
                }
                state = 1;
                // As read character which can not be
                // KEYWORD|IDENTIFIER|NUMERIC|VARIABLE so
                // pop_back will erase it from last index
                lexeme.pop_back();
                // If lexeme is back by one index so it should be too.
                it--;
            }
            // Found SUBTRACTION SIGN or Relational Operators
            else if (lexeme == "-")
            {
                state = 3;
            }
            // Found Start of Double Quoted String ot Single Quoted String
            else if (lexeme == "\"" || lexeme == "\'")
            {
                state = 4;
            }
            // Found Start of a Comment
            else if (lexeme == "#")
            {
                state = 5;
            }
            // Found Symbols or any INVALID RULE
            else
            {
                state = 2;
            }
            break;
        // It recieves the lexeme that has a WORD extracted from the iterator.
        // That word can be a KEYWORD|IDENTIFIER|NUMERIC|VARIABLE
        case 1:
            // If a KEYWORD
            if (regex_match(lexeme, keyWord))
            {
                if (lexeme == "function")
                {
                    token temp(lineNo,"", TokenType::FUNCTION);
                    tokens.push_back(temp);
                }
                else if (lexeme == "int")
                {
                    token temp(lineNo,"", TokenType::INT);
                    tokens.push_back(temp);
                }
                else if (lexeme == "char")
                {
                    token temp(lineNo,"", TokenType::CHAR);
                    tokens.push_back(temp);
                }
                else if (lexeme == "float")
                {
                    token temp(lineNo,"", TokenType::FLOAT);
                    tokens.push_back(temp);
                }
                else if (lexeme == "if")
                {
                    token temp(lineNo,"", TokenType::IF);
                    tokens.push_back(temp);
                }
                else if (lexeme == "else")
                {
                    token temp(lineNo,"", TokenType::ELSE);
                    tokens.push_back(temp);
                }
                else if (lexeme == "do")
                {
                    token temp(lineNo,"", TokenType::DO);
                    tokens.push_back(temp);
                }
                else if (lexeme == "until")
                {
                    token temp(lineNo,"", TokenType::UNTIL);
                    tokens.push_back(temp);
                }
                else if (lexeme == "then")
                {
                    token temp(lineNo,"", TokenType::THEN);
                    tokens.push_back(temp);
                }
                else if (lexeme == "read")
                {
                    token temp(lineNo,"", TokenType::READ);
                    tokens.push_back(temp);
                }
                else if (lexeme == "display")
                {
                    token temp(lineNo,"", TokenType::DISPLAY);
                    tokens.push_back(temp);
                }
                else if (lexeme == "displayline")
                {
                    token temp(lineNo,"", TokenType::DISPLAYLINE);
                    tokens.push_back(temp);
                }
                else if (lexeme == "return")
                {
                    token temp(lineNo,"", TokenType::RETURN);
                    tokens.push_back(temp);
                }
                else if (lexeme == "execute")
                {
                    token temp(lineNo,"", TokenType::EXECUTE);
                    tokens.push_back(temp);
                }
            }
            // If an IDENTIFIER
            else if (regex_match(lexeme, identifier))
            {
                token temp(lineNo,lexeme, TokenType::ID);
                tokens.push_back(temp);
            }
            // If a NUMERIC
            else if (regex_match(lexeme, numeric))
            {
                token temp(lineNo,lexeme, TokenType::NUM);
                tokens.push_back(temp);
            }
            // If a VARIABLE
            else if (regex_match(lexeme, variable))
            {
                // It is a mutual case where we have a separate $ as
                // Special Character or a Variable
                // If lexeme has only dollar sign and not a word
                // then distinguished as Special Character
                if (lexeme == "$")
                {
                    token temp(lineNo,lexeme, TokenType::DLR);
                    tokens.push_back(temp);
                }
                // It will be a variable
                else
                {
                    token temp(lineNo,lexeme, TokenType::VAR);
                    tokens.push_back(temp);
                }
            }
            // It will be any Invalid Rule such as $1234 or _1234
            // as variable name or identifier
            else if(lexeme == "\n")
            {
                lineNo++;
            }
            else if (lexeme != "\n" && lexeme !="\t")
            {
                 cout << "Invalid Rule Detected. ERROR!\n";
                 cout<<"ERROR Symbol: "<<lexeme<<"\n";
            }
            // Resetting State and Lexeme will read next single Char
            state = 0;
            lexeme.clear();
            lexeme.push_back(*it++);
            break;
        // For all the Symbols
        case 2:
            if (lexeme == ",")
            {
                token temp(lineNo,lexeme, TokenType::CMA);
                tokens.push_back(temp);
            }
            else if (lexeme == ":")
            {
                token temp(lineNo,lexeme, TokenType::CLN);
                tokens.push_back(temp);
            }
            else if (lexeme == ";")
            {
                token temp(lineNo,lexeme, TokenType::SCLN);
                tokens.push_back(temp);
            }
            else if (lexeme == "$")
            {
                token temp(lineNo,lexeme, TokenType::DLR);
                tokens.push_back(temp);
            }
            else if (lexeme == "(")
            {
                token temp(lineNo,lexeme, TokenType::OPTS);
                tokens.push_back(temp);
            }
            else if (lexeme == ")")
            {
                token temp(lineNo,lexeme, TokenType::CPTS);
                tokens.push_back(temp);
            }
            else if (lexeme == "+")
            {
                token temp(lineNo,lexeme, TokenType::ADD);
                tokens.push_back(temp);
            }
            else if (lexeme == "-")
            {
                token temp(lineNo,lexeme, TokenType::SBT);
                tokens.push_back(temp);
            }
            else if (lexeme == "*")
            {
                token temp(lineNo,lexeme, TokenType::MUL);
                tokens.push_back(temp);
            }
            else if (lexeme == "/")
            {
                token temp(lineNo,lexeme, TokenType::DIV);
                tokens.push_back(temp);
            }
            else if (lexeme == "%")
            {
                token temp(lineNo,lexeme, TokenType::MOD);
                tokens.push_back(temp);
            }
            else if (lexeme == "{")
            {
                token temp(lineNo,lexeme, TokenType::SBL);
                tokens.push_back(temp);
            }
            else if (lexeme == "}")
            {
                token temp(lineNo,lexeme, TokenType::CBL);
                tokens.push_back(temp);
            }
            else if (lexeme == "=")
            {
                token temp(lineNo,lexeme, TokenType::ASN);
                tokens.push_back(temp);
            }
            else if (lexeme == "<")
            {
                if(*it == '-')
                {
                    lexeme.push_back(*it++);
                    token temp(lineNo,lexeme, TokenType::FPASN);
                    tokens.push_back(temp);
                }
            }
            else if(lexeme == "\n")
            {
                lineNo++;
            }
            // If non defined Symbol found in the Language
            // Exception for NEWLINE and TAB
            else if (lexeme != "\n" && lexeme !="\t")
            {
                cout << "Invalid Rule Detected. ERROR!\n";
                cout<<"ERROR Symbol: "<<lexeme<<"\n";
            }
            state = 0;
            lexeme.clear();
            lexeme.push_back(*it++);
            break;
        // For Relational Operators
        case 3:
            if (*it == 'e')
            {
                if (*(it + 1) == 'q')
                {
                    // If it is Relational Operator
                    token temp(lineNo,"-eq", TokenType::EQL);
                    tokens.push_back(temp);
                    // Jumping Two indexex if case is TRUE
                    it = it + 2;
                }
                else
                {
                    //  Else it will push - as Subtraction sign
                    token temp(lineNo,lexeme, TokenType::SBT);
                    tokens.push_back(temp);
                }
            }
            else if (*it == 'n')
            {
                if (*(it + 1) == 'e')
                {
                    token temp(lineNo,"-ne", TokenType::NEQ);
                    tokens.push_back(temp);
                    it = it + 2;
                }
                else
                {
                    token temp(lineNo,lexeme, TokenType::SBT);
                    tokens.push_back(temp);
                }
            }
            else if (*it == 'l')
            {
                if (*(it + 1) == 't')
                {
                    token temp(lineNo,"-lt", TokenType::LTN);
                    tokens.push_back(temp);
                    it = it + 2;
                }
                else if (*(it + 1) == 'e')
                {
                    token temp(lineNo,"-le", TokenType::LTE);
                    tokens.push_back(temp);
                    it = it + 2;
                }
                else
                {
                    token temp(lineNo,lexeme, TokenType::SBT);
                    tokens.push_back(temp);
                }
            }
            else if (*it == 'g')
            {
                if (*(it + 1) == 't')
                {
                    token temp(lineNo,"-gt", TokenType::GTR);
                    tokens.push_back(temp);
                    it = it + 2;
                }
                else if (*(it + 1) == 'e')
                {
                    token temp(lineNo,"-ge", TokenType::GTE);
                    tokens.push_back(temp);
                    it = it + 2;
                }
                else
                {
                    token temp(lineNo,lexeme, TokenType::SBT);
                    tokens.push_back(temp);
                }
            }
            else
            {
                // If no Relational Operator found
                // It will be considered Subtration Sign
                token temp(lineNo,lexeme, TokenType::SBT);
                tokens.push_back(temp);
            }
            state = 0;
            lexeme.clear();
            lexeme.push_back(*it++);
            break;
        // For Strings.
        // Token Lexeme pair will be displayed and String
        // will be printed on terminal on runtime.
        case 4:
            if (lexeme == "\"")
            {
                // Parsing though double quoted string
                while (*it != '\"' && it < stream.end())
                {
                    lexeme.push_back(*it++);
                }
                // Checking if ending double quote found
                if (*it == '\"')
                {
                    // Jumping One index so next char to be pushed
                    // other then quotation sign
                    lexeme.push_back(*it++);
                    token temp(lineNo,lexeme, TokenType::DQSTR);
                    tokens.push_back(temp);
                }
                else
                {
                    cout << "\nWrong String Syntax. ERROR!\n";
                }
            }
            // Single Quoted String
            else
            {
                // Parsing through String
                while (*it != '\'' && it < stream.end())
                {
                    lexeme.push_back(*it++);
                }
                // Checking if ending single quote found.
                if (*it == '\'')
                {
                    // Jumping One index so next char to be pushed
                    // other then quotation sign
                    lexeme.push_back(*it++);
                    token temp(lineNo,lexeme, TokenType::SQSTR);
                    tokens.push_back(temp);
                }
                else
                {
                    cout << "\nWrong String Syntax. ERROR!\n";
                }
            }
            state = 0;
            lexeme.clear();
            lexeme.push_back(*it++);
            break;
        // For Comments.
        // Token Lexeme pair will be displayed and Comment
        // will be printed on terminal on runtime.
        case 5:
            if (*it == '~')
            {
                lexeme.push_back(*it++);
                while (*it != '~' && it < stream.end())
                {
                    lexeme.push_back(*it++);
                }
                // Checking if ending '~' sign found.
                if (*it == '~')
                {
                    //Pushing '~' sign to lexeme
                    lexeme.push_back(*it++);
                }
                // Checking if ending '#' sign found.
                if (*it == '#')
                {
                    //Pushing '#' sign to lexeme
                    lexeme.push_back(*it++);
                    token temp(lineNo,lexeme, TokenType::CMT);
                    tokens.push_back(temp);
                }
                else
                {
                    cout << "\nWrong Comment Syntax. ERROR!\n";
                }
            }
            state = 0;
            lexeme.clear();
            lexeme.push_back(*it++);
            break;
        default:
            lexeme.clear();
            lexeme.push_back(*it++);
            break;
        }
    }
    // End of file token
    tokens.push_back(token(-1,string(""), TokenType::END_OF_FILE));
}
lexer::lexer(const char filename[])
{
    // constructors: takes file name as an argument and store all
    // the contents of file into the class varible stream
    ifstream fin(filename);
    if (!fin) // if file not found
    {
        cout << "file not found" << endl;
    }
    else
    {
        char byte = 0;
        while (fin.get(byte))
        { // store file contents into stream
            if (byte != '\r')
                stream.push_back(byte);
        }
        stream.push_back(' '); // dummy spaces appended at the end
        stream.push_back(' ');
        // Tokenize function to get all the (token,lexeme) pairs
        Tokenize();
        // assigning tokens to iterator::pointer
        index = 0;
    }
}
lexer::lexer()
{
    index = -1;
}
void lexer::printRaw()
{
    // helper function to print the content of file
    vector<char>::iterator it = stream.begin();
    for (it = stream.begin(); it != stream.end(); it++)
    {
        cout << *it;
    }
    cout << endl;
}
token lexer::getNextToken()
{
    // this function will return single (token,lexeme) pair on each call
    // this is an iterator which is pointing to the beginning of vector of tokens
    token _token;
    if (index == tokens.size())
    {                       // return end of file if
        _token.lexeme = ""; // index is too large
        _token.tokenType = TokenType::END_OF_FILE;
    }
    else
    {
        _token = tokens[index];
        index = index + 1;
    }
    return _token;
}
void lexer::resetPointer()
{
    index = 0;
}
token lexer::peek(int howFar)
{
    if (howFar <= 0)
    { // peeking backward or in place is not allowed
        cout << "LexicalAnalyzer:peek:Error: non positive argument\n";
        exit(-1);
    }

    int peekIndex = index + howFar - 1;
    if (peekIndex > (tokens.size() - 1))
    {                                             // if peeking too far
        return token(-1,"", TokenType::END_OF_FILE); // return END_OF_FILE
    }
    else
        return tokens[peekIndex];
}
int lexer::noOfToken()
{
    return tokens.size();
}

token lexer::getToken(int pos)
{
    return tokens[pos];
}