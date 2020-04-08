#include "Interpreter.h"
#include <exception>
#include <algorithm>
using namespace std;

class SyntaxException : public std::exception {
public:
    // @override the exception::what()
    virtual const char *what() const throw()
    {
        return "Interpreter::SyntaxException";
	} 
};

string Interpreter::split_word(string s, int* index) {
    string word;
    while((s[*index] == ' ' || s[*index] == 10 || s[*index] == '\t') && s[*index] != '\0')
        (*index) ++;
    int start, end;
    start = *index;
    if(s[*index] == '(' || s[*index] == ',' || s[*index] == ')' || s[*index] == '=' || s[*index] == '>' || s[*index] == '<' || s[*index] == '*') {
        (*index) ++;
		if(s[*index] == '=' || s[*index] == '>')
			(*index) ++;
        end = *index;
        word = s.substr(start, end - start);
        return word;
    } else if(s[*index] == 39) {
        (*index) ++;
		if (s[*index] == 39) {
			(*index)++;
			return " ";
		}
        start ++;
        while(s[*index] != 39 && s[*index] != 0)
            (*index) ++;
        if(s[*index] == 39) {
            end = *index;
            (*index) ++;
            word = s.substr(start, end - start);
            return word;
        } else {
            return "";
        }
    }
    else {
        while(s[*index] != ' ' && s[*index] != '(' && s[*index] != 10 && 
			  s[*index] != ')' && s[*index] != ',' && s[*index] != 0 &&
			  s[*index] != '=' && s[*index] != '<' && s[*index] != '>' && s[*index] != '*')
            (*index) ++;
        end = *index;
        if(start != end)
            word = s.substr(start, end - start);
        else
            return "";
        return word;
    }
}

int Interpreter::interprete(string s) {
	int index = 0;
	string word;
	word = split_word(s, &index);
	if(strcmp(word.c_str(), "create") == 0) {
		word = split_word(s, &index);
		if(strcmp(word.c_str(), "table") == 0) {
			return interprete_create_table(s, index);
		} else if(strcmp(word.c_str(), "index") == 0) {
			return interprete_create_index(s, index);
		} else {
			cout << "Syntax Error for " << word << endl;
			return STATUS_FAIL;
		}
		return STATUS_FAIL;
	} else if(strcmp(word.c_str(), "select") == 0)
		return interprete_select(s, index);
	else if(word == "drop")
		return interprete_drop(s, index);
	else if(word == "delete")
		return interprete_delete(s, index);
	else if(word == "insert")
		return interprete_insert(s, index);
	else if(word == "quit")
		return STATUS_QUIT;
	else if(word == "execfile") {
		fileName = split_word(s, &index);
		cout << "Opening the file!" << endl;
		return STATUS_FILEREAD;
	} else if (word[0] == '#') {
		return STATUS_CONTINUE;
	}
	else {
		if(word != "") {
			cout << "Error! Command " << word << " not found!" << endl;
		}
		return STATUS_FAIL;
	}
	return STATUS_FAIL;
}

int Interpreter::interprete_create_table(string s, int index) {
	string word, primaryKey, tableName;
	word = split_word(s, &index);
	if(!word.empty())           //create table tablename
		tableName = word;
	else
	{
		cout << "Syntax Error for no table name" << endl;
		return 0;
	}

	word = split_word(s, &index);
	if(word.empty() || strcmp(word.c_str(), "(") != 0)
	{
		cout << "Error in syntax!" << endl;
		return 0;
	} else              // deal with attribute list
	{
		word = split_word(s, &index);
		std::vector<Attribute> attributeVector;
		while(!word.empty() && strcmp(word.c_str(), "primary") != 0 && strcmp(word.c_str(), ")") != 0)
		{
			string attributeName = word;
			int type = 0;
			bool ifUnique = false;
			// deal with the data type
			word = split_word(s, &index);
			if(strcmp(word.c_str(), "int") == 0)
				type = -1;
			else if(strcmp(word.c_str(), "float") == 0)
				type = -2;
			else if(strcmp(word.c_str(), "char") == 0)
			{
				word = split_word(s, &index);
				if(strcmp(word.c_str(), "("))
				{
					cout << "Syntax Error: unknown data type" << endl;
					return 0;
				}
				word = split_word(s, &index);
				istringstream convert(word);
				if(!(convert >> type))
				{
					cout << "Syntax error : illegal number in char()" << endl;
					return 0;
				}
				word = split_word(s, &index);
				if(strcmp(word.c_str(), ")"))
				{
					cout << "Syntax Error: unknown data type" << endl;
					return 0;
				}
			} else
			{
				cout << "Syntax Error: unknown or missing data type!" << endl;
				return 0;
			}
			word = split_word(s, &index);
			if(strcmp(word.c_str(), "unique") == 0)
			{
				ifUnique = true;
				word = split_word(s, &index);
			}
			Attribute attr(attributeName, type, ifUnique);
			attributeVector.push_back(attr);
			if(strcmp(word.c_str(), ",") != 0)
			{
				if(strcmp(word.c_str(), ")") != 0) {
					cout << "Syntax Error for ,!" << endl;
					return 0;
				} else
					break;
			}

			word = split_word(s, &index);
		}
		int primaryKeyLocation = 0;
		if(strcmp(word.c_str(), "primary") == 0)    // deal with primary key
		{
			word = split_word(s, &index);
			if(strcmp(word.c_str(), "key") != 0)
			{
				cout << "Error in syntax!" << endl;
				return 0;
			} else
			{
				word = split_word(s, &index);
				if(strcmp(word.c_str(), "(") == 0)
				{
					word = split_word(s, &index);
					primaryKey = word;
					int i = 0;
					for(i = 0; i<attributeVector.size(); i++)
					{
						if(primaryKey == attributeVector[i].get_name())
						{
							attributeVector[i].set_unique(true);
							break;
						}

					}
					if(i == attributeVector.size())
					{
						cout << "Syntax Error: primaryKey does not exist in attributes " << endl;
						return 0;
					}
					primaryKeyLocation = i;
					word = split_word(s, &index);
					if(strcmp(word.c_str(), ")") != 0)
					{
						cout << "Error in syntax!" << endl;
						return 0;
					}
				} else
				{
					cout << "Error in syntax!" << endl;
					return 0;
				}
				word = split_word(s, &index);
				if(strcmp(word.c_str(), ")") != 0)
				{
					cout << "Error in syntax!" << endl;
					return 0;
				}
			}
		} else if(word.empty())
		{
			cout << "Syntax Error: ')' absent!" << endl;
			return 0;
		}
		ap->tableCreate(tableName, &attributeVector, primaryKey, primaryKeyLocation);
		return 1;
	}
}

int Interpreter::interprete_create_index(string s, int index) {
	string indexName, tableName, attributeName, word;
	word = split_word(s, &index);
	if(!word.empty())           //create index indexname
		indexName = word;
	else {
		cout << "Error in syntax!" << endl;
		return 0;
	}

	word = split_word(s, &index);
	try {
		if(strcmp(word.c_str(), "on") != 0)
			throw SyntaxException();
		word = split_word(s, &index);
		if(word.empty())
			throw SyntaxException();
		tableName = word;
		word = split_word(s, &index);
		if(strcmp(word.c_str(), "(") != 0)
			throw SyntaxException();
		word = split_word(s, &index);
		if(word.empty())
			throw SyntaxException();
		attributeName = word;
		word = split_word(s, &index);
		if(strcmp(word.c_str(), ")") != 0)
			throw SyntaxException();
		ap->indexCreate(indexName, tableName, attributeName);
		return 1;
	} catch(SyntaxException&) {
		cout << "Syntax Error!" << endl;
		return 0;
	}
}

int Interpreter::interprete_select(string s, int index) {
	vector<string> attrSelected;
	string tableName, word;
	word = split_word(s, &index);
	if(strcmp(word.c_str(), "*") != 0)  // only accept select *
	{
		while(strcmp(word.c_str(), "from")) {
			if(strcmp(word.c_str(), ",")) 
				attrSelected.push_back(word);
			word = split_word(s, &index);
		}
	} else {
		word = split_word(s, &index);
	}
	if(strcmp(word.c_str(), "from") != 0) {
		cout << "Error in syntax!" << endl;
		return 0;
	}

	word = split_word(s, &index);
	if(!word.empty())
		tableName = word;
	else {
		cout << "Error in syntax!" << endl;
		return 0;
	}

	// condition extricate
	word = split_word(s, &index);
	if(word.empty())    // without condition
	{
		if(attrSelected.size() == 0)
			ap->recordShow(tableName); // nullptr
		else
			ap->recordShow(tableName, &attrSelected);
		return 1;
	} else if(strcmp(word.c_str(), "where") == 0)
	{
		string attributeName = "";
		string value = "";
		int operate = Condition::OPERATOR_EQUAL;
		std::vector<Condition> conditionVector;
		word = split_word(s, &index);        //col1
		while(1) {
			try {
				if(word.empty())
					throw SyntaxException();
				attributeName = word;
				word = split_word(s, &index);
				if(strcmp(word.c_str(), "<=") == 0)
					operate = Condition::OPERATOR_LESS_EQUAL;
				else if(strcmp(word.c_str(), ">=") == 0)
					operate = Condition::OPERATOR_MORE_EQUAL;
				else if(strcmp(word.c_str(), "<") == 0)
					operate = Condition::OPERATOR_LESS;
				else if(strcmp(word.c_str(), ">") == 0)
					operate = Condition::OPERATOR_MORE;
				else if(strcmp(word.c_str(), "=") == 0)
					operate = Condition::OPERATOR_EQUAL;
				else if(strcmp(word.c_str(), "<>") == 0)
					operate = Condition::OPERATOR_NOT_EQUAL;
				else
					throw SyntaxException();
				word = split_word(s, &index);
				if(word.empty()) // no value
					throw SyntaxException();
				value = word;
				Condition c(attributeName, value, operate);
				conditionVector.push_back(c);
				word = split_word(s, &index);
				if(word.empty()) // no condition -> over
					break;
				if(strcmp(word.c_str(), "and") != 0)
					throw SyntaxException();
				word = split_word(s, &index);
			} catch(SyntaxException&) {
				cout << "Syntax Error!" << endl;
				return 0;
			}
		}
		if(attrSelected.size() == 0) {
			ap->recordShow(tableName, nullptr, &conditionVector);
		}
		else {
			ap->recordShow(tableName, &attrSelected, &conditionVector);
		}
		return 1;
	}
	else {
		return 0;
	}
}

int Interpreter::interprete_drop(string s, int index) {
	string word = split_word(s, &index);
	try
	{
		if(word == "table")
		{
			word = split_word(s, &index);
			if(!word.empty())
			{
				ap->tableDrop(word);
				//cout << "drop table " << word;
				return 1;
			} else
				throw SyntaxException();
		} else if(word == "index")
		{
			word = split_word(s, &index);
			if(!word.empty())
			{
				ap->indexDrop(word);
				//cout << "drop index" << word;
				return 1;
			} else
				throw SyntaxException();
		} else
			throw SyntaxException();
	} catch(SyntaxException&)
	{
		cout << "Syntax Error!" << endl;
		return 0;
	}
}

int Interpreter::interprete_delete(string s, int index) {
	string tableName, word;
	word = split_word(s, &index);
	try
	{
		if(word != "from")
			throw SyntaxException();

		word = split_word(s, &index);
		if(!word.empty())
			tableName = word;
		else
			throw SyntaxException();

	} catch(SyntaxException&)
	{
		cout << "Syntax Error!" << endl;
		return 0;
	}

	//delete condition
	word = split_word(s, &index);
	if(word.empty())// without condition
	{
		ap->recordDelete(tableName);
		return 1;
	} else if(word == "where")
	{
		string attributeName = "";
		string value = "";
		int operate;
		vector<Condition> ConditionVec;
		word = split_word(s, &index);
		while(1) {
			try {
				if(word.empty())
					throw SyntaxException();
				attributeName = word;
				word = split_word(s, &index);
				if(word == "<=")
					operate = Condition::OPERATOR_LESS_EQUAL;
				else if(word == ">=")
					operate = Condition::OPERATOR_MORE_EQUAL;
				else if(word == "<")
					operate = Condition::OPERATOR_LESS;
				else if(word == ">")
					operate = Condition::OPERATOR_MORE;
				else if(word == "=")
					operate = Condition::OPERATOR_EQUAL;
				else if(word == "<>")
					operate = Condition::OPERATOR_NOT_EQUAL;
				else
					throw SyntaxException();
				word = split_word(s, &index);
				if(word.empty())
					throw SyntaxException();
				value = word;
				Condition cond(attributeName, value, operate);
				ConditionVec.push_back(cond);

				word = split_word(s, &index);
				if(word.empty())
					break;
				else if(word == "and")
				{
					word = split_word(s, &index);
				} else
					throw SyntaxException();

			} catch(SyntaxException&)
			{
				cout << "Syntax Error!" << endl;
				return 0;
			}
		}
		ap->recordDelete(tableName, &ConditionVec);
		return 1;
	} else {
		return 0;
	}
}

int Interpreter::interprete_insert(string s, int index) {
	string tableName, word;
	vector<string> ValueVec;
	word = split_word(s, &index);
	try {
		if(word != "into")
			throw SyntaxException();
		word = split_word(s, &index);
		if(word.empty())
			throw SyntaxException();
		tableName = word;
		word = split_word(s, &index);
		if(word != "values")
			throw SyntaxException();
		word = split_word(s, &index);
		if(word != "(")
			throw SyntaxException();
		word = split_word(s, &index);
		while(!word.empty() && word != ")") {
			ValueVec.push_back(word);
			word = split_word(s, &index);
			if(word == ",")
				word = split_word(s, &index);
		}
		if(word != ")")
			throw SyntaxException();
	} catch(SyntaxException&) {
		cout << "Syntax Error!" << endl;
		return 0;

	}
	ap->recordInsert(tableName, &ValueVec);
	return 1;
}