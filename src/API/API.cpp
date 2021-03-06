#include "API.h"
#include <string>

#define UNKNOWN_FILE 1
#define TABLE_FILE 1
#define INDEX_FILE 1
using namespace std;

void API::tableDrop(string tableName)
{
    if (!tableExist(tableName)) return;
    
    vector<string> indexNameVector;
    
    //get all index in the table, and drop them all
    indexNameListGet(tableName, &indexNameVector);
    for (int i = 0; i < indexNameVector.size(); i++)
    {
        printf("%s", indexNameVector[i].c_str());
        indexDrop(indexNameVector[i]);
    }
    
    //delete a table file
    if(rm->tableDrop(tableName))
    {
        //delete a table information
        cm->drop_table(tableName);
        printf("Drop table %s successfully\n", tableName.c_str());
    }
}


void API::indexDrop(string indexName)
{
    if (cm->is_index_exist(indexName) != INDEX_FILE)
    {
        printf("There is no index %s \n", indexName.c_str());
        return;
    }
    
    //delete a index file
    if (rm->indexDrop(indexName))
    {
        
        //get type of index
        int indexType = cm->get_index_type(indexName);
        
        //delete a index information
        cm->drop_index(indexName);
        
        //delete a index tree
        im->drop(rm->indexFileNameGet(indexName), indexType);
        printf("Drop index %s successfully\n", indexName.c_str());
    }
}

void API::indexCreate(string indexName, string tableName, string attributeName)
{
    if (cm->is_index_exist(indexName) == INDEX_FILE)
    {
        cout << "There is index " << indexName << " already" << endl;
        return;
    }
    
    if (!tableExist(tableName)) return;
    
    vector<Attribute> attributeVector;
    cm->get_attribute(tableName, attributeVector);
    int i;
    int type = 0;
    for (i = 0; i < attributeVector.size(); i++)
    {
        if (attributeName == attributeVector[i].get_name())
        {
            if (!attributeVector[i].is_unique())
            {
                cout << "the attribute is not unique" << endl;
                
                return;
            }
            type = attributeVector[i].get_type();
            break;
        }
    }
    
    if (i == attributeVector.size())
    {
        cout << "there is not this attribute in the table" << endl;
        return;
    }
    
    //CatalogManager to add a index information
    cm->create_index(indexName, tableName, attributeName, type);
        
    //get type of index
    int indexType = cm->get_index_type(indexName);
        
	if(indexName == "name_idx") {
		int debug = 1;
	}

    //indexManager to create a index tress
    im->create(rm->indexFileNameGet(indexName), indexType);
        
    //recordManager insert already record to index
    rm->indexRecordAllAlreadyInsert(tableName, indexName, attributeName);
    printf("Create index %s successfully\n", indexName.c_str());
}

void API::tableCreate(string tableName, vector<Attribute>* attributeVector, string primaryKeyName, int primaryKeyLocation)
{
    if(cm->is_table_exist(tableName) == TABLE_FILE)
    {
        cout << "There is a table " << tableName << " already" << endl;
        return;
    }
    
    cm->create_table(tableName,primaryKeyName ,*attributeVector);
    printf("Create table %s successfully\n", tableName.c_str());
    
    if (primaryKeyName != "")
    {
        //get a primary key
        string indexName = primaryIndexNameGet(tableName);
        indexCreate(indexName, tableName, primaryKeyName);
    }
}

void API::recordShow(string tableName, vector<string>* attributeNameVector)
{
    vector<Condition> conditionVector;
    recordShow(tableName, attributeNameVector, &conditionVector);
}

void API::recordShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector)
{
    if (cm->is_table_exist(tableName) == TABLE_FILE)
    {
        int num = 0;
        vector<Attribute> attributeVector;
        attributeGet(tableName, &attributeVector);
        
        vector<string> allAttributeName;
        if (attributeNameVector == NULL) {
            for (Attribute attribute : attributeVector)
            {
                allAttributeName.insert(allAttributeName.end(), attribute.get_name());
            }
            
            attributeNameVector = &allAttributeName;
        }
        
        //print attribute name you want to show
        tableAttributePrint(attributeNameVector);
        
        for (string name : (*attributeNameVector))
        {
            int i = 0;
            for (i = 0; i < attributeVector.size(); i++)
            {
                if (attributeVector[i].get_name() == name)
                {
                    break;
                }
            }
            
            if (i == attributeVector.size())
            {
                cout << "the attribute which you want to print is not exist in the table" << endl;
                return;
            }
        }
        
        int blockOffset = -1;
        if (conditionVector != NULL)
        {
            for (Condition condition : *conditionVector)
            {
                int i = 0;
                for (i = 0; i < attributeVector.size(); i++)
                {
                    if (attributeVector[i].get_name() == condition.attributeName)
                    {
                        if (condition.operate == Condition::OPERATOR_EQUAL && cm->get_index(tableName, attributeVector[i].get_name())!= "")
                        {
							Value x(0, 0, "", 0);
							//if the attribute has a index
							if (attributeVector[i].get_type() == -2) {//float
								Value a(0, stof(condition.value), "", attributeVector[i].get_type());
								x = a;
							}
							else if (attributeVector[i].get_type() == -1) {//int
								Value a(stoi(condition.value), 0, "", attributeVector[i].get_type());
								x = a;
							}
							else {
								Value a(0, 0, condition.value, attributeVector[i].get_type());
								x = a;
							}
                            blockOffset = im->search(rm->indexFileNameGet(cm->get_index(tableName, attributeVector[i].get_name())), x);
                        }
                        break;
                    }
                }
                
                if (i == attributeVector.size())
                {
                    cout << "the attribute is not exist in the table" << endl;
                    return;
                }
            }
        }
        
        if (blockOffset == -1)
        {
            //cout << "if we con't find the block by index,we need to find all block" << endl;
            num = rm->recordAllShow(tableName, attributeNameVector,conditionVector);
        }
        else
        {         
            //find the block by index,search in the block
            num = rm->recordBlockShow(tableName, attributeNameVector, conditionVector, blockOffset);
        }
        
        printf("%d records selected\n", num);
    }
    else
    {
        cout << "There is no table " << tableName << endl;
    }
}

void API::recordInsert(string tableName, vector<string>* recordContent)
{
    if (!tableExist(tableName)) return;
    
    string indexName;
    
    //deal if the record could be insert (if index is exist)
    vector<Attribute> attributeVector;
    
    vector<Condition> conditionVector;
    
    attributeGet(tableName, &attributeVector);
    for (int i = 0 ; i < attributeVector.size(); i++)
    {
        indexName = cm->get_index(tableName, attributeVector[i].get_name());
        if (indexName != "")
        {
			Value x(0,0,"",0);
            //if the attribute has a index
			if (attributeVector[i].get_type() == -2) {//float
				Value a( 0,stof((*recordContent)[i]), "", attributeVector[i].get_type());
				x = a;
			}
			else if (attributeVector[i].get_type() == -1) {//int
				Value a( stoi((*recordContent)[i]),0, "", attributeVector[i].get_type());
				x = a;
			}
			else {
				Value a(0, 0, (*recordContent)[i],attributeVector[i].get_type());
				x = a;
			}
            int blockoffest = im->search(rm->indexFileNameGet(indexName), x);
            
            if (blockoffest != -1)
            {
                //if the value has exist in index tree then fail to insert the record
                cout << "insert fail because index value exist" << endl;
                return;
            }
        }
        else if (attributeVector[i].is_unique())
        {
            //if the attribute is unique but not index
            Condition condition(attributeVector[i].get_name(), (*recordContent)[i], Condition::OPERATOR_EQUAL);
            conditionVector.insert(conditionVector.end(), condition);
        }
    }
    
    if (conditionVector.size() > 0)
    {
        for (int i = 0; i < conditionVector.size(); i++) {
            vector<Condition> conditionTmp;
            conditionTmp.insert(conditionTmp.begin(), conditionVector[i]);
            
            int recordConflictNum =  rm->recordAllFind(tableName, &conditionTmp);
            if (recordConflictNum > 0) {
                cout << "insert fail because unique value exist" << endl;
                return;
            }

        }
    }
    
    char recordString[2000];
    memset(recordString, 0, 2000);
    
    //CatalogManager to get the record string
    cm->get_record(tableName, *recordContent,recordString);
    
    //RecordManager to insert the record into file; and get the position of block being insert
    int recordSize = cm->get_record_size(tableName);
    int blockOffset = rm->recordInsert(tableName,recordString ,recordSize);
    
    if(blockOffset >= 0)
    {
        recordIndexInsert(recordString, recordSize, &attributeVector, blockOffset, tableName);
        cm->add_record(tableName);

        printf("insert record into table %s successful\n", tableName.c_str());
    }
    else
    {
        cout << "insert record into table " << tableName << " fail" << endl;
    }
}

void API::recordDelete(string tableName)
{
    vector<Condition> conditionVector;
    recordDelete(tableName, &conditionVector);
}

void API::recordDelete(string tableName, vector<Condition>* conditionVector)
{
    if (!tableExist(tableName)) return;
    
    int num = 0;
    vector<Attribute> attributeVector;
    attributeGet(tableName, &attributeVector);

    int blockOffset = -1;
    if (conditionVector != NULL)
    {
        for (Condition condition : *conditionVector)
        {
            if (condition.operate == Condition::OPERATOR_EQUAL)
            {
                for (Attribute attribute : attributeVector)
                {
                    if (cm->get_index(tableName,attribute.get_name()) != "" && attribute.get_name() == condition.attributeName)
                    {
						Value x(0, 0, "", 0);
						//if the attribute has a index
						if (attribute.get_type() == -2) {//float
							Value a(0, stof(condition.value), "", attribute.get_type());
							x = a;
						}
						else if (attribute.get_type() == -1) {//int
							Value a(stoi(condition.value), 0, "", attribute.get_type());
							x = a;
						}
						else {
							Value a(0, 0, (condition.value), attribute.get_type());
							x = a;
						}

                        blockOffset = im->search(rm->indexFileNameGet(cm->get_index(tableName, attribute.get_name())),x);
                    }
                }
            }
        }
    }

    
    if (blockOffset == -1)
    {
        //if we con't find the block by index,we need to find all block
        num = rm->recordAllDelete(tableName, conditionVector);
    }
    else
    {
        //find the block by index,search in the block
        num = rm->recordBlockDelete(tableName, conditionVector, blockOffset);
    }
    
    //delete the number of record in in the table
    cm->delete_record(tableName, num);
    printf("delete %d record in table %s\n", num, tableName.c_str());
}

int API::recordNumGet(string tableName)
{
    if (!tableExist(tableName)) return 0;
    
    return cm->get_record_num(tableName);
}

int API::recordSizeGet(string tableName)
{
    if (!tableExist(tableName)) return 0;
    
    return cm->get_record_size(tableName);
}

int API::typeSizeGet(int type)
{
    return cm->get_type_size(type);
}

int API::indexNameListGet(string tableName, vector<string>* indexNameVector)
{
    if (!tableExist(tableName)) {
        return 0;
    }
    return cm->get_index_name(tableName, *indexNameVector);
}

void API::allIndexAddressInfoGet(vector<Index> *indexNameVector)
{
    cm->get_all_index(*indexNameVector);
    for (int i = 0; i < (*indexNameVector).size(); i++)
    {
        (*indexNameVector)[i].set_name( rm->indexFileNameGet((*indexNameVector)[i].get_name()));
    }
}

int API::attributeGet(string tableName, vector<Attribute>* attributeVector)
{
    if (!tableExist(tableName)) {
        return 0;
    }
    return cm->get_attribute(tableName, *attributeVector);
}

void API::recordIndexInsert(char* recordBegin,int recordSize, vector<Attribute>* attributeVector,  int blockOffset, string tableName)
{
    char* contentBegin = recordBegin;
    for (int i = 0; i < (*attributeVector).size() ; i++)
    {
        int type = (*attributeVector)[i].get_type();
        int typeSize = typeSizeGet(type);
        if (cm->get_index(tableName,(*attributeVector)[i].get_name()) != "")
        {
            indexInsert(cm->get_index(tableName, (*attributeVector)[i].get_name()), contentBegin, type, blockOffset);
        }
        
        contentBegin += typeSize;
    }
}

void API::indexInsert(string indexName, char* contentBegin, int type, int blockOffset)
{
    string content= "";
    stringstream tmp;
    //if the attribute has index

	if(type == Attribute::TYPE_INT)
	{
		int value = *((int*)contentBegin);
		tmp << value;
	} else if (type == Attribute::TYPE_FLOAT) {
        float value = *((float* )contentBegin);
        tmp << value;
    } else {
        char value[255];
        memset(value, 0, 255);
        memcpy(value, contentBegin, cm->get_type_size(type));
        string stringTmp = value;
        tmp << stringTmp;
    }

	
    tmp >> content;
	Value x(0, 0, "", 0);
	//if the attribute has a index
	if (type == -2) {//float
		Value a(0, stof(content), "", type);
		x = a;
	}
	else if (type == -1) {//int
		Value a(stoi(content), 0, "", type);
		x = a;
	}
	else {
		Value a(0, 0, content, type);
		x = a;
	}
    im->insert(rm->indexFileNameGet(indexName),x, blockOffset);
}

void API::recordIndexDelete(char* recordBegin,int recordSize, vector<Attribute>* attributeVector, int blockOffset, string tableName)
{
    char* contentBegin = recordBegin;
    for (int i = 0; i < (*attributeVector).size() ; i++)
    {
        int type = (*attributeVector)[i].get_type();
        int typeSize = typeSizeGet(type);
        
        string content= "";
        stringstream tmp;
        
        if (cm->get_index(tableName, (*attributeVector)[i].get_name()) != "")
        {
            //if the attribute has index
            if (type == Attribute::TYPE_INT)
            {
                int value = *((int*)contentBegin);
                tmp << value;
            }
            if (type == Attribute::TYPE_FLOAT)
            {
                float value = *((float* )contentBegin);
                tmp << value;
            }
            else
            {
                char value[255];
                memset(value, 0, 255);
                memcpy(value, contentBegin, cm->get_type_size(type));
                string stringTmp = value;
                tmp << stringTmp;
            }
			Value x(0, 0, "", 0);
			tmp >> content;
			//if the attribute has a index
			if (type == -2) {//float
				Value a(0, stof(content), "", type);
				x = a;
			}
			else if (type == -1) {//int
				Value a(stoi(content), 0, "", type);
				x = a;
			}
			else {
				Value a(0, 0, content, type);
				x = a;
			}
            
            im->erase(rm->indexFileNameGet(cm->get_index(tableName,(*attributeVector)[i].get_name())),x);

        }
        contentBegin += typeSize;
    }

}

int API::tableExist(string tableName)
{
    if (cm->is_table_exist(tableName) != TABLE_FILE)
    {
        cout << "There is no table " << tableName << endl;
        return 0;
    }
    else
    {
        return 1;
    }
}

string API::primaryIndexNameGet(string tableName)
{
    return  tableName;
}

void API::tableAttributePrint(vector<string>* attributeNameVector)
{
    int i = 0;
    for ( i = 0; i < (*attributeNameVector).size(); i++)
    {
        printf("| %-15s", (*attributeNameVector)[i].c_str());
    }
    if (i != 0)
        printf("\n");
}

