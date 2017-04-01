#ifndef CSE250_FALL2015_USERFUNCTION_H
#define CSE250_FALL2015_USERFUNCTION_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stack>
#include <queue>
#include "Parser.h"

using namespace std;

// This class is not required, but might be helpful in writing your interpreter
class UserFunction {

public:

    //UserFunction();
    UserFunction(queue<string>); //user defined function without params
    UserFunction(queue<string>, vector<string> parameters); //user defined function with params
    double call(vector<double> parameters);

    void interpretScript(queue<string>);
    int getPrecedence(string line);
    vector<string> infixToPostfix(vector<string>); //convert from infix to postfix
    double computePostfix(vector<string>); // separate function for computing infix notation
    void putSpaceBetween(string& lnToSpace); //put spaces between operators/ands in infix expression
    string getRtOfAssign(string rassign);// Return the right side of an assignment
    void removeVecSpaces(vector<string>& vecToFix); //Removes unneeded spaces caused by shitty 'putSpacesBetween' method
    void addVarValues(vector<string>& v); // This finds operands that are user-defined variables and replaces their name their value
    string getOutput() {return outputFile;}; //Gets output of function to be written to file
    double getReturnValue() {return returnValue;}; //Get of return value user defined function
    bool getConParams() {return containsParams;};
    vector<string> getParams() {return functionParams;};
    void addFuncValues(vector<string>& v);
    bool hasParams(string& s);
    vector<string> getParams(string s);
    void setFunctionMap(map<string, UserFunction> m) {functionMap = m;}; //Setter to give previously defined function to new function
    void setPrevFuncs(map<string, queue<string>> sF);
    void prevFunction(string s);
    void setFuncParams(vector<string> i) {functionParams = i; };
    //void prevFunction(string fileLine);
    void handleNegs(vector<string>& lineVec);

private:
     map<string, double> variableMap; // a map of variable names to Variable values
    map<string, UserFunction> functionMap; // a map of user defined functions
    // stack to remember nesting of if blocks
    stack<string> infix; // I need to change this name late, because it is used for postfix too
    queue<string> writeQueue;
    stack<double> post; //Stack used for computing postfix
    string outputFile;
    double returnValue; //Return value of a user defined function
    vector<string> functionContents;// contents of a parameratized function
    vector<string> functionParams;// Parameters (not the arguments themselves)
    bool containsParams = false;
    vector<string> theArgs; //the arguments while computing a user defined function
    map<string, queue<string>> storedFunctions;
    string writtenOutput;
    bool ifTrue = false;



};



#endif //CSE250_FALL2015_USERFUNCTION_H
