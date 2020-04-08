#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H
#include <string>
#include <vector>
#include "../lib/Condition.h"
#include "../lib/Attribute.h"
#include "../RecordManager/RecordManager.h"
#include "../BufferManager/BufferManager.h"
#include "../lib/common.h"

class API;
class RecordManager {
public:
	RecordManager() {}
	BufferManager buffer;
	API *api;

	bool tableDrop(std::string tableName);
	bool indexDrop(std::string indexName);

	int recordInsert(std::string tableName, char* record, int recordSize);

	int recordAllShow(std::string tableName, std::vector<std::string>* arrayOfAttributeName, std::vector<Condition>* arrayOfCondition);
	int recordBlockShow(std::string tableName, std::vector<std::string>* arrayOfAttributeName, std::vector<Condition>* arrayOfCondition, int blockOffset);

	int recordAllFind(std::string tableName, std::vector<Condition>* arrayOfCondition);

	int recordAllDelete(std::string tableName, std::vector<Condition>* arrayOfCondition);
	int recordBlockDelete(std::string tableName, std::vector<Condition>* arrayOfCondition, int blockOffset);

	int indexRecordAllAlreadyInsert(std::string tableName, std::string indexName, std::string attributeName);

	std::string tableFileNameGet(std::string tableName);
	std::string indexFileNameGet(std::string indexName);
private:
	int recordBlockShow(std::string tableName, std::vector<std::string>* arrayOfAttributeName, std::vector<Condition>* arrayOfCondition, blockNode* block);
	int recordBlockFind(std::string tableName, std::vector<Condition>* arrayOfCondition, blockNode* block);
	int recordBlockDelete(std::string tableName, std::vector<Condition>* arrayOfCondition, blockNode* block);
	int indexRecordBlockAlreadyInsert(std::string tableName, std::string indexName, blockNode* block, std::string attributeName);

	bool recordConditionFit(char* recordBegin, int recordSize, std::vector<Attribute>* arrayOfAttribute, std::vector<Condition>* arrayOfCondition);
	void recordPrint(char* recordBegin, int recordSize, std::vector<Attribute>* arrayOfAttribute, std::vector<std::string> *arrayOfAttributeName);
	bool contentConditionFit(char* content, int type, Condition* condition);
	void contentPrint(char * content, int type);

	char* transferString(std::string x);
};
#endif
