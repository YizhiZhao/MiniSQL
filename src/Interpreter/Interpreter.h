#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <iostream>
#include <string>
#include <vector>
#include "../lib/Condition.h"
#include "../lib/Attribute.h"
#include "../Api/API.h"

#define STATUS_OK 1
#define STATUS_FAIL 0
#define STATUS_FILEREAD 2
#define STATUS_CONTINUE 1000
#define STATUS_QUIT 10000

class Interpreter{
public:
	Interpreter() :
		ap(nullptr) {}
	virtual ~Interpreter() {}
    API *ap;
    std::string fileName;
    std::string split_word(std::string s, int* index);
    int interprete(std::string s);
private:
	int interprete_create_table(std::string s, int index);
	int interprete_create_index(std::string s, int index);
	int interprete_select(std::string s, int index);
	int interprete_drop(std::string s, int index);
	int interprete_delete(std::string s, int index);
	int interprete_insert(std::string s, int index);
};

#endif