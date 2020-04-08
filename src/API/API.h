#ifndef API_H
#define API_H

#include "../lib/Attribute.h"
#include "../lib/Condition.h"
#include "../lib/common.h"
#include "../lib/Index.h"
#include "../CatalogManager/CatalogManager.h"
#include "../IndexManager/IndexManager.h"
#include "../RecordManager/RecordManager.h"
#include <string>
#include <cstring>
#include <vector>
#include <stdio.h>

class CatalogManager;
class RecordManager;
class IndexManager;
class API{
public:
    RecordManager *rm;
    CatalogManager *cm;
    IndexManager *im;
	API(){}
	~API(){}
    
    
    void tableDrop(std::string tableName);
    void tableCreate(std::string tableName, std::vector<Attribute>* attributeVector, std::string primaryKeyName, int primaryKeyLocation);

    void indexDrop(std::string indexName);
	void indexCreate(std::string indexName, std::string tableName, std::string attributeName);

    void recordShow(std::string tableName, std::vector<std::string>* attributeNameVector = NULL);
	void recordShow(std::string tableName, std::vector<std::string>* attributeNameVector, std::vector<Condition>* conditionVector);

	void recordInsert(std::string tableName, std::vector<std::string>* recordContent);

	void recordDelete(std::string tableName);
	void recordDelete(std::string tableName, std::vector<Condition>* conditionVector);

	int recordNumGet(std::string tableName);
	int recordSizeGet(std::string tableName);

	int typeSizeGet(int type);
    
    void allIndexAddressInfoGet(std::vector<Index> *indexNameVector);
    
    int attributeNameGet(std::string tableName, std::vector<std::string>* attributeNameVector);
	int attributeTypeGet(std::string tableName, std::vector<std::string>* attributeTypeVector);
    int attributeGet(std::string tableName, std::vector<Attribute>* attributeVector);

    void indexValueInsert(std::string indexName, std::string value, int blockOffset);
    
    void indexInsert(std::string indexName, char* value, int type, int blockOffset);
    void recordIndexDelete(char* recordBegin,int recordSize, std::vector<Attribute>* attributeVector, int blockOffset, std::string tableName);
    void recordIndexInsert(char* recordBegin,int recordSize, std::vector<Attribute>* attributeVector, int blockOffset, std::string tableName);
    
	int isIndexTrue(std::string tableName, std::string attributeName, std::string indexName) {
		return cm->is_index_true(tableName, attributeName, indexName);
	}
	std::string getIndex(std::string tableName, std::string attributeName) {
		return cm->get_index(tableName, attributeName);
	}
private:
    int tableExist(std::string tableName);
    int indexNameListGet(std::string tableName, std::vector<std::string>* indexNameVector);
    std::string primaryIndexNameGet(std::string tableName);
    void tableAttributePrint(std::vector<std::string>* name);
};

#endif
