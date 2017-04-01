#include "Interpreter.h"
#include <iomanip>

void Interpreter::interpretScript(ifstream& inputFile, ofstream& outputFile)
{
    string lineFromFile, varName, curLn, lineFromFunction;
    int lineNumber; // var name is the name of a variable and temp is temp line for curLn

    while (getline(inputFile, lineFromFile)) {

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
            }// If line has parameters, get those params and store them

            putSpaceBetween(curLn); //Put spaces between the current line to fit into vector correctly
            vector<string> lineVec = tokenize(curLn, " "); //vector of infix
            removeVecSpaces(lineVec);
            addFuncValues(lineVec);//Replace a function call with its value
            addVarValues(lineVec);

            vector<string> val  = infixToPostfix(lineVec); //INFIX to POSTFIX seems to work
            value = computePostfix(val);
            variableMap.insert(make_pair(varName, value)); // Add variable name and value
        } else if (lineType == 2) { //The line starts with a user-defined variable or function name. ie y = x+1

            if(lineFromFile.find_first_of('=') == std::string::npos) { // If a function name
                vector<string> args = getParams(lineFromFile);
                vector<double> args2;
                for(int i = 0; i != args.size(); i++) {
                    args2.push_back(variableMap.find(args.at(i))->second);
                }

                string temp;
                while(lineFromFile.size() != 0) {
                    string token = getNextSymbol(lineFromFile);
                    if(token == "(") {
                        break;
                    } else {
                        temp = temp + token;
                    }
                }

                lineFromFile = temp;


                UserFunction found = functionMap.find(lineFromFile)->second; //Find already defined variable


                if(found.getConParams()) {

                    found.call(args2);
                    writeQueue.push(found.getOutput());
                    //We need to figure out what the arguments are and pass them in

                }//if the function has parameters
                else {
                    writeQueue.push(found.getOutput());
                }

            } else {

                varName = lineFromFile.substr(0, min(lineFromFile.find_first_of(" "),  lineFromFile.find_first_of("="))); //Get var name

                lineFromFile = getRtOfAssign(lineFromFile);// Get the right side of assignment statement
                putSpaceBetween(lineFromFile); //Put saces between operator and operand
                vector<string> theLineVec = tokenize(lineFromFile, " "); //Vector of operator and operand and possibly some random spaces
                removeVecSpaces(theLineVec);//Remove empty elements from vector
                addVarValues(theLineVec);
               addFuncValues(theLineVec);
                vector<string> val2  = infixToPostfix(theLineVec); //INFIX to POSTFIX seems to work
                value = computePostfix(val2); //Compute the postfix value
                // cout << "var name: " << varName;
                auto found = variableMap.find(varName); //Find already defined variable
                found->second = value; //Give said variable new value
            }


            //TO DO: USER DEFINED FUNCTIONS
            //FOR FUNCTION NAME REMEBER WE NEED TO USE THE FUNCTION MAP TO GRAB FUNCTIONS
            //WE WILL BE USING THE CALL FUNCTION HERE OBVIOUSLY
        } else if (lineType == 3) { // document.write is called

            lineFromFile = lineFromFile.substr(lineFromFile.find_first_of("("), lineFromFile.size());

            if(lineFromFile.find_first_of("\"") == string::npos) { //Add variable value to queue for output
                lineFromFile =  lineFromFile.substr(lineFromFile.find_first_of("(")+1, lineFromFile.find_first_of(")")-1);
                double num = variableMap.find(lineFromFile)->second;
                string toPush = to_string(num);

                toPush = toPush.substr(0, toPush.find_last_not_of('0'));
                writeQueue.push(toPush);
            } else { //For computing
                lineFromFile =  lineFromFile.substr(lineFromFile.find_first_of("\"")+1, lineFromFile.size());
                lineFromFile =  lineFromFile.substr(0, lineFromFile.find_first_of("\""));
                writeQueue.push(lineFromFile);
            }
        } else if (lineType == 4) { // Beginning of a new user function definition
            queue<string> storedFunction;//This will be passed into the new function
            storedFunction.push(lineFromFile);//Store first line of function
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

                while (getline(inputFile, lineFromFunction)) {
                    if(lineFromFunction == "}") { // If function defition is over
                        storedFunction.push(lineFromFunction);
                        lineNumber++;
                        break;
                    } else { // Function definition is not over
                        userFunctionQ.push(lineFromFunction);
                        storedFunction.push(lineFromFunction);
                        lineNumber++;
                    }
                }

                newFunc = new UserFunction(userFunctionQ, params);

            } else if(!(hasParams(lineFromFile))) {
                while (getline(inputFile, lineFromFunction)) {
                    if(lineFromFunction == "}") { // If function defition is over
                        storedFunction.push(lineFromFunction);
                        lineNumber++;
                        break;
                    } else { // Function definition is not over
                        userFunctionQ.push(lineFromFunction);
                        storedFunction.push(lineFromFunction);
                        lineNumber++;
                    }
                    //This is where you will add the contents
                }//Until function defition ends add all lines to be run through a userFunction
                newFunc = new UserFunction(userFunctionQ);
            } //If the function def doesn't have params

            storedFunctions.insert(make_pair(funcName, storedFunction));
            newFunc->setPrevFuncs(storedFunctions);
            newFunc->setFunctionMap(functionMap);
            functionMap.insert(make_pair(funcName, *newFunc));//Add function name and function to map

        } else if (lineType == 5) { // return statement of a user-defined function. WE SHOULD NOT REACH THIS IN THIS CLASS
            //Return is always a single variable (can have “return x” but no “return 1”)
        } else if (lineType == 6) { // A single '}' on a line

        } else if (lineType == 7) { //he beginning of an if block.
            double var1, var2;
            string comparator, currentLine;
            //         functionMap.find(lineFromFile)->second

            string s = lineFromFile;
            while(s.size() != 0) {
                if (getNextSymbol(s) == "(") {
                    var1 = variableMap.find(getNextSymbol(s))->second;
                    comparator = getNextSymbol(s);
                    var2 = variableMap.find(getNextSymbol(s))->second;
                }
            }//This gets the variables and comparator

            if (comparator == ">") {
                if(var1 > var2) {
                    ifTrue = true;
                    //Do nothing
                } else {
                    while (getline(inputFile, currentLine)) {
                        if(currentLine == "}" ) { // If function defition is over
                            lineNumber++;
                            break;
                        } else {
                            lineNumber++;
                        }
                    }
                }
            } else {
                if(var1 < var2) {
                    ifTrue = true;
                    //Do nothing
                } else {
                    while (getline(inputFile, currentLine)) {
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
            if(ifTrue == false && ifElse == false) { //if if statment was false
                // do nothing

            } else {

                while (getline(inputFile, currentLine)) {
                    if(currentLine == "}" ) { // If function defition is over
                        lineNumber++;
                        break;
                    } else {
                        lineNumber++;
                    }
                }

            }

            ifTrue = false;
            ifElse = false;
        } else if (lineType == 9) { //A line of the form "} else if(<condition>) {"
if(ifTrue == false){
              double var1, var2;
            string comparator, currentLine;
            //         functionMap.find(lineFromFile)->second

            string s = lineFromFile;
            while(s.size() != 0) {
                if (getNextSymbol(s) == "(") {
                    var1 = variableMap.find(getNextSymbol(s))->second;
                    comparator = getNextSymbol(s);
                    var2 = variableMap.find(getNextSymbol(s))->second;
                }
            }//This gets the variables and comparator

            if (comparator == ">") {
                if(var1 > var2) {
                    ifElse = true;
                    //Do nothing
                } else {
                    while (getline(inputFile, currentLine)) {
                        if(currentLine == "}" ) { // If function defition is over
                            lineNumber++;
                            break;
                        } else {
                            lineNumber++;
                        }
                    }
                }
            } else {
                if(var1 < var2) {
                    ifElse = true;
                    //Do nothing
                } else {
                    while (getline(inputFile, currentLine)) {
                        if(currentLine == "}" ) { // If function defition is over
                            lineNumber++;
                            break;
                        } else {
                            lineNumber++;
                        }
                    }
                }
            }

            }
        }
        // Use your interpreter to execute each line

    }
    // write the result from the return statement of the program into the output file
    //We need to pop back the entire queue, put it in a string, then output to this file
    while(!(writeQueue.empty())) {
        writtenOutput = writtenOutput + writeQueue.front();
        writeQueue.pop();
    }
    outputFile << writtenOutput;
}

vector<string> Interpreter::infixToPostfix(vector<string> infixLine)
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

int Interpreter::getPrecedence(string line)
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

double Interpreter::computePostfix(vector<string> x)
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

void Interpreter::putSpaceBetween(string& lnToSpace)
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
}//it's supposed
// to put spaces between each operator and operand, so they can all be put in a vector

string Interpreter::getRtOfAssign(string rassign)
{
    rassign = rassign.substr(rassign.find_first_of("=")+1,rassign.size()); // right side of the assignment IT WORKS
    return rassign.substr(rassign.find_first_not_of(" "), rassign.size());
} // Returns the right side of an assigment

void Interpreter::removeVecSpaces(vector<string>& vecToFix)
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

void Interpreter::addVarValues(vector<string>& v)
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

void Interpreter::addFuncValues(vector<string>& v)
{
    string toFind = v.at(0);
    auto found = functionMap.find(toFind);


    if(found != functionMap.end()) {// If the function exists
        if(found->second.getConParams()) { //If function has params
            vector<string> args = theArgs; // these are the args the user has fgiven
            vector<double> args2; //Where the double type of the args will be

            for(int i = 0; i != args.size(); i++) {
                cout << "Interpreter.cpp: Variable map value is: "  << variableMap.find(args.at(i))->second << endl;
                args2.push_back(variableMap.find(args.at(i))->second);
            }//Change args to double type



            found->second.call(args2); //Add args to user defined function and compute

            // writeQueue.push(to_string(found->second.getReturnValue())); //Get the return value of the user returned function and push it into output
            v.clear();

            v.push_back(to_string(found->second.getReturnValue()));//Switches right of assignment with updated valueu

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
    }

}

bool Interpreter::hasParams(string& s)  // to see a function being defined has parameters
{
    string temp = s;
    while(temp.size() != 0) {
        if(getNextSymbol(temp) == "(") {
            return (getNextSymbol(temp) != ")");
        }
    }
    return false;
}// Return true if function has paramters; false otherwise

vector<string> Interpreter::getParams(string s)
{
    vector<string> temp;
    getNextSymbol(s);
    getNextSymbol(s);

    while(s.size() != 0) {
        string symbol = getNextSymbol(s);
        if (symbol != ")" && symbol != "," && symbol != "{") {
            temp.push_back(symbol);
        }
    }

    return temp;
}//Finds out the param symbol of a new user defined function


