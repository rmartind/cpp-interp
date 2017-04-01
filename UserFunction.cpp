#include "UserFunction.h"
#include <string>
#include <vector>
#include <map>
//#include <fstream>
#include <stack>
#include <queue>
#include "Parser.h"
#include <algorithm>
//#include <iterator>



//Essentially here we have another interpreter inside of an interpreter. It will follow the same logic I guess

UserFunction::UserFunction(queue<string> fakeFile)
{
    interpretScript(fakeFile);
}

UserFunction::UserFunction(queue<string> fakeFile, vector<string> parameters)
{
    functionParams = parameters;//Store paramters of a function
    containsParams = true;

     while (!(fakeFile.empty())) {
    functionContents.push_back(fakeFile.front());
    fakeFile.pop();
    }//Add the contents of function definition to functionContents

 // We call this to alter the parameters in the queue string
}// this is called in the beginning of a function definition

double UserFunction::call(vector<double> parameters)
{
    queue<string> queueToPass;


    for(int i = 0; i < functionContents.size(); i++ ){
        string line = functionContents.at(i);
        string tempRealLine = line;
        string updatedLine;
        string tempLine;
        while(line.size() != 0) {
        string character = getNextSymbol(line);
        if (getLineType(tempRealLine) == 3 && (tempRealLine.find_first_of("\"") != string::npos )) {
        updatedLine = tempRealLine;
        }
        else if(std::find(functionParams.begin(), functionParams.end(), character) == end(functionParams)){ //If
            tempLine = updatedLine + " " + character;//this is new
            if(getLineType(tempLine)== 1 && variableMap.find(character)!= variableMap.end() || (
                    getLineType(tempLine)== 2 && tempLine.find_first_of("=") != string::npos && variableMap.find(character)!= variableMap.end())){//this is new
                updatedLine = updatedLine + " " + to_string(variableMap.find(character)->second);
            }else {//this is new
                //tempLine = updatedLine + " " + character;//bran new
                //if(getLineType(tempLine)==3 && (character == "\?" || character == "<" || character == ">" || character == "/")){
                  //  updatedLine = updatedLine + character;
               // } else {
            updatedLine = updatedLine + " " + character; //Removed a space from here. I may need to add it back
            }
            //this is new

        } else {
            for(int j = 0; j < functionParams.size(); j++){
                if(character == functionParams.at(j)){
                    updatedLine = updatedLine + to_string(parameters.at(j));
                }
            }
        }
    }
        queueToPass.push(updatedLine);
        interpretScript(queueToPass);//This used to be ran at the end after queue was finished
        queueToPass.pop();
    }






    //interpretScript(queueToPass);

    //interpretScript(queueToPass);
    //Get contents of function
    //Get paramters
    //replace the parameters in the contents of the function with the arguments
    //run interscript with contents



    //if next token is a param, replace it with it's double param
    return 0;
}//if a user define functioned has parameters, we will first use this function to replace the parameters default
//characters with the doubles in the vector
// at the end of this function we will call intersprict with the updated parameters





void UserFunction::interpretScript(queue<string> inputFile)
{
//Maybe we need to create a new interpreter
    string lineFromFile, varName, curLn, lineFromFunction;
    //string writtenOutput;

    int lineNumber = 0; // var name is the name of a variable and temp is line for curLn

    while (!(inputFile.empty())) {

        lineFromFile = inputFile.front();
        inputFile.pop();

        lineNumber++;
        LINE_TYPE lineType = getLineType(lineFromFile); // Check Parser.h for the different line types
        curLn = lineFromFile; //Redudant can remove later just for clarity right now
        double value = 0.0;

        if(lineType == 0) { //Blank line
            //Ignore the blank line
        } else if (lineType == 1) { // variable definition with def ie var x = 8

            int endOfVar = curLn.find_first_of('r'); //end of "var"
            curLn = curLn.substr(endOfVar+2, curLn.size()-1);
            varName = curLn.substr(0, min(curLn.find_first_of(" "), curLn.find_first_of("="))); // Get variable name
            curLn = getRtOfAssign(curLn); //Gets right side of assignment statement


            string temp = curLn;
            if(hasParams(temp)) {
                theArgs = getParams(temp);
            }



            putSpaceBetween(curLn); //Put spaces between the current line to fit into vector correctly
            vector<string> lineVec = tokenize(curLn, " "); //vector of infix
            removeVecSpaces(lineVec);

            addFuncValues(lineVec); //Replace a function call with its value
            addVarValues(lineVec);




            handleNegs(lineVec); //This is new MAY CAUSE PROBLEMS



            vector<string> val  = infixToPostfix(lineVec); //INFIX to POSTFIX seems to work


            value = computePostfix(val);
            variableMap.insert(make_pair(varName, value)); // Add variable name and value

        } else if (lineType == 2) { //The line starts with a user-defined variable or function name. ie y = x+1
            //IMPORTANT DO WE NEED TO DEAL WITH USER DEFINED VARIABLE BEING ASSIGNED USER DEFINED FUNCTION ie x = myFunction()


            if(lineFromFile.find_first_of('=') == std::string::npos) { // If a function name


                vector<string> args = getParams(lineFromFile);

                vector<double> args2;


                for(int i = 0; i != args.size(); i++)
                {
                    args2.push_back(variableMap.find(args.at(i))->second);
                }



                //string actualLine = lineFromFile;
                //cout << "actual line:" << actualLine << endl;

                lineFromFile = lineFromFile.substr(0 , lineFromFile.find_first_of("(")-1);
                string temp;
                for(int i = 0;i < lineFromFile.size(); i++) {
                    if(lineFromFile.at(i) != ' ') {
                        temp = temp + lineFromFile.at(i);
                    }
                }
                lineFromFile = temp;

                //We will add LineFromFile to function HETERELGFJRGOJPRFGOPJRGOJOFGJORGOJOPTJGPGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
                prevFunction(lineFromFile);





                auto found = functionMap.find(lineFromFile)->second; //Find already defined variable SEG FAULT HERE


                        if(found.getConParams()) {

                            found.call(args2);
                            writeQueue.push(found.getOutput());
                    //We need to figure out what the arguments are and pass them in

                        }else {
                    writeQueue.push(found.getOutput());
                }



             }else {

                varName = lineFromFile.substr(lineFromFile.find_first_not_of(" "), lineFromFile.size()-1);
                varName = varName.substr(0, min(varName.find_first_of(" "),  varName.find_first_of("=")));//Get var name

                lineFromFile = getRtOfAssign(lineFromFile);// Get the right side of assignment statement


                string temp = lineFromFile;//this is new
            if(hasParams(temp)) { //this is new
                theArgs = getParams(temp); //this is new
            }

                putSpaceBetween(lineFromFile); //Put saces between operator and operand
                vector<string> theLineVec = tokenize(lineFromFile, " "); //Vector of operator and operand and possibly some random spaces
                removeVecSpaces(theLineVec);//Remove empty elements from vector
                addFuncValues(theLineVec);
                addVarValues(theLineVec);

                 handleNegs(theLineVec);// this is new MAY CAUSE PROBLEMS

                vector<string> val2  = infixToPostfix(theLineVec); //INFIX to POSTFIX seems to work
                value = computePostfix(val2); //Compute the postfix value
                // cout << "var name: " << varName;
                auto found = variableMap.find(varName); //Find already defined variable
                found->second = value; //Give said variable new value
            }

            //END
            //TO DO: USER DEFINED FUNCTIONS
            //FOR FUNCTION NAME REMEBER WE NEED TO USE THE FUNCTION MAP TO GRAB FUNCTIONS
            //WE WILL BE USING THE CALL FUNCTION HERE OBVIOUSLY
        } else if (lineType == 3) { // document.write is called


            lineFromFile = lineFromFile.substr(lineFromFile.find_first_of("("), lineFromFile.size());


            if(lineFromFile.find_first_of("\"") == string::npos) { //Add variable value to queue for output
                lineFromFile =  lineFromFile.substr(lineFromFile.find_first_of("(")+1, lineFromFile.find_first_of(")")-1);

                string removeSp;
                for(int i = 0; i < lineFromFile.size(); i++){
                    if(lineFromFile.at(i) != ' ') {
                        removeSp = removeSp + lineFromFile.at(i);
                    }
                }
                lineFromFile = removeSp;

                double num;
                try
                {
                    num = stod(lineFromFile);
                }catch( std::invalid_argument )
                {
                    num = variableMap.find(lineFromFile)->second;
                }

                string toPush = to_string(num);

                toPush = toPush.substr(0, toPush.find_last_not_of('0'));

                writeQueue.push(toPush);
            } else { //For computing


                lineFromFile =  lineFromFile.substr(lineFromFile.find_first_of("\"")+1, lineFromFile.size());
                lineFromFile =  lineFromFile.substr(0, lineFromFile.find_first_of("\""));
                writeQueue.push(lineFromFile);
            }
        } else if (lineType == 4) { // Beginning of a new user function definition

            queue<string> storedFunction;//this is new

            storedFunction.push(lineFromFile);//this is new

            string funcName;
            int endOfFunc = lineFromFile.find_first_of(' '); //end of "var"
            lineFromFile = lineFromFile.substr(endOfFunc+1, lineFromFile.size()-1);
            funcName = lineFromFile.substr(0, lineFromFile.find_first_of("(")); //Get function name


            queue<string> userFunctionQ;
            UserFunction* newFunc;
            if(hasParams(lineFromFile)) { //If the function def has params\
                //TO DO: same as else if, except we use the user function with two parameters
                //       to add the function parameters. We also need to get every parameter and pass it as a vector
                vector<string> params = getParams(lineFromFile);// Vector of the functions parameters

                while (!(inputFile.empty())) {
                    lineFromFile = inputFile.front();
                    inputFile.pop();

                    if(lineFromFunction == "}") { // If function defition is over
                    storedFunction.push(lineFromFunction); //this is new
                        lineNumber++;
                        break;
                    } else { // Function definition is not over
                        storedFunction.push(lineFromFunction); //this is new
                        userFunctionQ.push(lineFromFunction);
                        lineNumber++;
                    }
                    }


                    newFunc = new UserFunction(userFunctionQ, params);

            } else if(!(hasParams(lineFromFile))) {
                while (!(inputFile.empty())) {
                    lineFromFile = inputFile.front();
                    inputFile.pop();
                    if(lineFromFunction == "}") { // If function defition is over
                        storedFunction.push(lineFromFunction);//this is new
                        lineNumber++;
                        break;
                    } else { // Function definition is not over
                        storedFunction.push(lineFromFunction);//this is new
                        userFunctionQ.push(lineFromFunction);
                        lineNumber++;
                    }

                    newFunc = new UserFunction(userFunctionQ);
                }//Until function defition ends add all lines to be run through a userFunction

                //Declare a new function to put into function map. Later on we can use the call function

            } //If the function def doesn't have params

             storedFunctions.insert(make_pair(funcName, storedFunction));//this is new
             newFunc->setPrevFuncs(storedFunctions); //this is new
            functionMap.insert(make_pair(funcName, *newFunc));//Add function name and function to map



        } else if (lineType == 5) { // return statement of a user-defined function.
            lineFromFile = lineFromFile.substr(lineFromFile.find_first_of('n')+1, lineFromFile.size()-1);
            lineFromFile = lineFromFile.substr(lineFromFile.find_first_not_of(' '), lineFromFile.size()-1);
            returnValue = variableMap.find(lineFromFile)->second;
        } else if (lineType == 6) { // A single '}' on a line

        } else if (lineType == 7) { //he beginning of an if block.

            double var1, var2;
            string comparator, currentLine;
            //         functionMap.find(lineFromFile)->second

            string s = lineFromFile;
            while(s.size() != 0)
            {
                if (getNextSymbol(s) == "(") {
                    var1 = variableMap.find(getNextSymbol(s))->second;
                    comparator = getNextSymbol(s);
                    var2 = variableMap.find(getNextSymbol(s))->second;
                }
            }//This gets the variables and comparator

            if (comparator == ">")
            {
                if(var1 > var2) {
                    ifTrue = true;
                    //Do nothing
                } else {
                    while (!(inputFile.empty())) {
                        currentLine = inputFile.front();
                        inputFile.pop();
                        if(currentLine == "}" ) { // If function defition is over
                            lineNumber++;
                            break;
                        } else {
                            lineNumber++;
                        }
                    }
                }
            } else {
                if(var1 < var2)
                {
                    ifTrue = true;
                    //Do nothing
                } else {
                    while (!(inputFile.empty()))
                    {
                        currentLine = inputFile.front();
                        inputFile.pop();
                        if(currentLine == "}" ) { // If function defition is over
                            lineNumber++;
                            break;
                        } else {
                            lineNumber++;
                        }
                    }
                }
            }

        } else if (lineType == 8) { //A line of the form "} else {

            string currentLine;
            if(ifTrue == false)   //if if statment was false
            {
                // do nothing

            } else {

                while (!(inputFile.empty()))
                    {
                        currentLine = inputFile.front();
                        inputFile.pop();
                    if(currentLine == "}" ) { // If function defition is over
                        lineNumber++;
                        break;
                    } else {
                        lineNumber++;
                    }
                }

            }

        } else if (lineType == 9) { //A line of the form "} else if(<condition>) {"

        }
        // Use your interpreter to execute each line
    }

    // write the result from the return statement of the program into the output file
    //We need to pop back the entire queue, put it in a string, then output to this file
    while(!(writeQueue.empty())) {
        writtenOutput = writtenOutput + writeQueue.front();
        writeQueue.pop();
    }

    outputFile = writtenOutput; //There is a function to get this output called get output for Interpreter to use
    //cout << outputFile;
}

vector<string> UserFunction::infixToPostfix(vector<string> infixLine)
{
    vector<string> output;
    string leftParen = " ";

    for (auto i = 0; i != infixLine.size(); i++) {
        string curChar = infixLine.at(i); //Not a char though

        if (curChar == "(") {
            leftParen = "(";
            infix.push(curChar);
        } else if (curChar == ")") {
            while(infix.top() != "(") {
                output.push_back(infix.top());
                infix.pop();
            }
            infix.pop();
        } //If and else if here handles parens

        if (isNumber(curChar)) { //t is an operand
            //output = output + curChar;//add operand to output
            output.push_back(curChar);
        } else if(curChar == "+" || curChar == "-" || curChar == "*" || curChar == "/") { // t is an operator
            while (!(infix.empty()) && getPrecedence(curChar) <= getPrecedence(infix.top())) { // If current operator is of lesser or equal value to the operator on top of stack
                //output = output + infix.top(); // Throw it in the output
                output.push_back(infix.top());
                infix.pop(); // Now it leaves the stack. Sayonara operator.

            }
            infix.push(curChar); //Put operator on stack
        }
    }// Iterate through right side of assignment

    while(!(infix.empty())) {
        //output = output + infix.top();
        output.push_back(infix.top());
        infix.pop();
    }// Add remaining operators to output

    return output;
} // Converts an infix expression to a postfix expression--well that's what I fucking want at least. Who knows

int UserFunction::getPrecedence(string line)
{

    if (line == "+" || line == "-") {
        return 0;
    } else if (line == "*" || line == "/") {
        return 1;
    } else {
        return -1;
    }
    //The enum isn't even needed. can just return ints.
} //Gets the precedence level of an operator to be used for infix to postfix conversion

double UserFunction::computePostfix(vector<string> x)
{
    double done = 0.0;

    for (auto i = 0; i != x.size(); i++) {
        string curChar = x.at(i); //Not a char though
        double total;

        if (isNumber(curChar)) { //t is an operand
            double push = stod(curChar);
            post.push(push);

        } else if(curChar == "*" || curChar == "/" || curChar == "+" || curChar == "-") {
            double num2 = post.top();
            post.pop();
            double num1 = post.top();
            post.pop();

            if (curChar == "*") {
                total = num1 * num2;
            } else if (curChar == "/") {
                total = num1 / num2;
            } else if (curChar == "+") {
                total = num1 + num2;
            } else if (curChar == "-") {
                total = num1 - num2;
            }
            post.push(total);
        } //When encountering operator, sum up to closest values in the stack
    }

    done = post.top();
    post.pop();
    return done;
} //Compute the value of a postfix expression

void UserFunction::putSpaceBetween(string& lnToSpace)
{
    lnToSpace = " " + lnToSpace + " ";
    string temp;
    for (int i = 0; i != lnToSpace.size(); i++) {
        if (lnToSpace.at(i) == '+' || lnToSpace.at(i) == '-' || lnToSpace.at(i) == '/' ||
                lnToSpace.at(i) == '*' || lnToSpace.at(i) == ')' || lnToSpace.at(i) == '(') {
            if (lnToSpace.at(i-1) != ' ' && lnToSpace.at(i+1) != ' ') {
                temp = temp + " " + lnToSpace.at(i) + " ";
            } else if (lnToSpace.at(i+1) != ' ') {
                temp = temp + lnToSpace.at(i) + " ";
            } else if (lnToSpace.at(i-1) != ' ') {
                temp = temp + " " + lnToSpace.at(i);
            } else {
                temp = temp + lnToSpace.at(i);
            }
        } else {
            temp = temp + lnToSpace.at(i);
        }

    } //Put spaces between every operand and operator
    lnToSpace = temp;
}//This is a buggy complicated piece of shit that has caused nothing but headaches. It's supposed
// to put spaces between each operator and operand, so they can all be put in a vector

string UserFunction::getRtOfAssign(string rassign)
{
    rassign = rassign.substr(rassign.find_first_of("=")+1,rassign.size()); // right side of the assignment IT WORKS
    return rassign.substr(rassign.find_first_not_of(" "), rassign.size());
} // Returns the right side of an assigment

void UserFunction::removeVecSpaces(vector<string>& vecToFix)
{
    vector<string> positions;
    for (int i = 0; i != vecToFix.size(); i++) {
        if(!(vecToFix.at(i).empty())) {
            positions.push_back(vecToFix.at(i));
        }
    }//Remove empty space elements from vector
    vecToFix = positions;
}// Removes troublesome spaces added by 'putSpaceBetween' member. So that way there won't be empty elements in the vector which is only
//supposed to contain operators and operands

void UserFunction::addVarValues(vector<string>& v)
{
    for (int i = 0; i < v.size(); i++) {
        string toFind = v.at(i);
        auto found = variableMap.find(toFind);
        if (found != variableMap.end()) {
            string replaced = to_string(found->second);
            v[i] = replaced;
        }
    }

}// Finds any variables names on the right side of an assignment statement.
// It replaces the name with the variables current value, so the value of the right side can be properly computed


void UserFunction::addFuncValues(vector<string>& v)
{
        vector<double> args2;//This is new MAY CAUSE PROBLEMS

        string toFind = v.at(0);




        prevFunction(toFind);
        auto found = functionMap.find(toFind);


    if(found != functionMap.end()) {// If the function exists
        if(found->second.getConParams()) { //If function has params
            vector<string> args = theArgs; // these are the args the user has fgiven
          //  vector<double> args2; //Where the double type of the args will be


            for(int i = 0; i != args.size(); i++) {
                try {
                    double num = stod(args.at(i));

                    args2.push_back(num);
                } catch( std::invalid_argument ) {
                args2.push_back(variableMap.find(args.at(i))->second);
            }//Change args to double type
            }// IF WE This causes problems, this line was just the code in the catch line


            found->second.call(args2); //Add args to user defined function and compute

           // writeQueue.push(to_string(found->second.getReturnValue())); //Get the return value of the user returned function and push it into output

           v.clear();
            v.push_back(to_string(found->second.getReturnValue()));//Switches right of assignment with updated value
            writtenOutput = writtenOutput + found->second.getOutput();

        } else { //Else function doesn't have params
            // UserFunction found = functionMap.find(toFind)->second;
            if (functionMap.find(toFind)!= functionMap.end()) {
                string replaced = to_string(found->second.getReturnValue());
                v.clear();
                v.push_back(replaced);
                writtenOutput = writtenOutput + found->second.getOutput();
            }
        } //If function is found, apply its return value
    }else {
    }

}

bool UserFunction::hasParams(string& s)  // to see a function being defined has parameters
{
    string temp = s;
    while(temp.size() != 0) {
        if(getNextSymbol(temp) == "(") {
            return (getNextSymbol(temp) != ")");
        }
    }
    return false;
}// Return true if function has paramters; false otherwise

vector<string> UserFunction::getParams(string s)
{//This function fucks up negative values. Isn't that robust?


    vector<string> temp;
    getNextSymbol(s);
    getNextSymbol(s);

    while(s.size() != 0) {
        string symbol = getNextSymbol(s);

    try {
        double number = stod(symbol);
        temp.push_back(symbol);
    } catch (std::invalid_argument){


        if (symbol != ")" && symbol != "," && symbol != "{" && symbol != " "){
            if(symbol == "-") {
                string negative = symbol + getNextSymbol(s);
                temp.push_back(negative);
            }else {
            temp.push_back(symbol);
            }
        }
    }//This had a MAJOR change of having the try-catch statement If rolling back, just leave the code in catch statement
    }

    return temp;
}//Finds out the param symbol of a new user defined function

void UserFunction::setPrevFuncs(map<string, queue<string>> sF) {
        storedFunctions= sF;
}

void UserFunction::prevFunction(string fileLine)
{
    if(storedFunctions.find(fileLine) != storedFunctions.end()) {
        string tempLine = fileLine, lineFromFunction;

        auto func = storedFunctions.find(tempLine)->second;

        while(!(func.empty())) {//While there are still lines
            string actualLine = func.front();
            tempLine = func.front();
            func.pop();
            string theName;
            int endOfFunc = tempLine.find_first_of(' '); //end of "var"
            tempLine = tempLine.substr(endOfFunc+1, tempLine.size()-1);
            theName = tempLine.substr(0, tempLine.find_first_of("(")); //Get function name

         //   cout << theName << ":" << endl;
            queue<string> queueForFunc;
            UserFunction* newFunc;
            if(hasParams(actualLine)) { //If the function def has params\
                //TO DO: same as else if, except we use the user function with two parameters
                //       to add the function parameters. We also need to get every parameter and pass it as a vector
                vector<string> params = getParams(tempLine);// Vector of the functions parameters

                while (!(func.empty())) {
                    lineFromFunction = func.front();
                    func.pop();

                    if(lineFromFunction == "}") { // If function defition is over
                        //lineNumber++;
                        break;
                    } else { // Function definition is not over
                        queueForFunc.push(lineFromFunction);
                        //lineNumber++;
                    }
                }

                newFunc = new UserFunction(queueForFunc, params);

            } else if(!(hasParams(actualLine))) {
                while (!(func.empty())) {
                    lineFromFunction =func.front();
                    func.pop();
                    if(lineFromFunction == "}") { // If function defition is over
                        //lineNumber++;
                        break;
                    } else { // Function definition is not over
                        queueForFunc.push(lineFromFunction);
                        //lineNumber++;
                    }
                }//Until function defition ends add all lines to be run through a userFunction
                newFunc = new UserFunction(queueForFunc);
                //Declare a new function to put into function map. Later on we can use the call function

            } //If the function def doesn't have params
            //Add function name and function to map
            functionMap.insert(make_pair(theName, *newFunc));
        }
    }

} //This recreates a function stored in the stored functions queue. It creates it and adds it to the function map NOT EVEN NEEDED BUT TOO LATE NOW

void UserFunction::handleNegs(vector<string>& lineVec) {
if (lineVec.at(0)== "-"){
                lineVec.insert(lineVec.begin()+2, ")");
                         lineVec.insert(lineVec.begin(), "0");
                         lineVec.insert(lineVec.begin(), "(");
            }

            for (int i = 1; i < lineVec.size(); i++){
                if((lineVec.at(i) == "+"|| lineVec.at(i) == "-"|| lineVec.at(i) == "*"|| lineVec.at(i) == "/") &&
                    lineVec.at(i+1) == "-") {
                        lineVec.insert(lineVec.begin()+i+3, ")");
                        double num = stod(lineVec.at(i+2));
                        double numtoAdd = 0;
                        lineVec.insert(lineVec.begin()+i+1, to_string(numtoAdd));
                        lineVec.insert(lineVec.begin()+i+1, "(");

                    }
            }

}


