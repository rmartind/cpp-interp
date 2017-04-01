#ifndef UNTITLED_INTERPRETER_H
#define UNTITLED_INTERPRETER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stack>
#include <queue>
#include "Parser.h"
#include "UserFunction.h"

using namespace std;

class Interpreter {

private:
    // suggestions:
    map<string, double> variableMap; // a map of variable names to Variable values
    map<string, UserFunction> functionMap; // a map of user defined functions
    // stack to remember nesting of if blocks
    stack<string> infix; // I need to change this name late, because it is used for postfix too
    queue<string> writeQueue;
    stack<double> post; //Stack used for computing postfix
    vector<string> theArgs; //the arguments while computing a user defined function
    map<string, queue<string>> storedFunctions;
    string writtenOutput;
    bool ifTrue = false;
    bool ifElse = false;

public:
    map<string, UserFunction> getfunctionMap() {return functionMap;};
    void interpretScript(ifstream&, ofstream&);
    int getPrecedence(string line);
    vector<string> infixToPostfix(vector<string>); //convert from infix to postfix
    double computePostfix(vector<string>); // separate function for computing infix notation
    void putSpaceBetween(string& lnToSpace); //put spaces between operators/ands in infix expression
    string getRtOfAssign(string rassign);// Return the right side of an assignment
    void removeVecSpaces(vector<string>& vecToFix); //Removes unneeded spaces caused by shitty 'putSpacesBetween' method
    void addVarValues(vector<string>& v); // This finds operands that are user-defined variables and replaces their name their value
    void addFuncValues(vector<string>& v); //This finds "operands" that are user-defined functions and replaces their name their value
    bool hasParams(string&); // to see a function being defined has parameters
    vector<string> getParams(string s);

};


#endif //UNTITLED_INTERPRETER_H
