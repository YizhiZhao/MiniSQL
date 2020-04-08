#ifndef CONDITION_H
#define CONDITION_H
#include <string>
#include <sstream>

class Condition
{
    
public:
    const static int OPERATOR_EQUAL = 0; // "="
    const static int OPERATOR_NOT_EQUAL = 1; // "<>"
    const static int OPERATOR_LESS = 2; // "<"
    const static int OPERATOR_MORE = 3; // ">"
    const static int OPERATOR_LESS_EQUAL = 4; // "<="
    const static int OPERATOR_MORE_EQUAL = 5; // ">="
    
    Condition(std::string a,std::string v,int o);
    
    std::string attributeName;
    std::string value;           // the value to be compared
    int operate;            // the type to be compared
    
    bool ifRight(int content);
    bool ifRight(float content);
    bool ifRight(std::string content);
};

#endif
