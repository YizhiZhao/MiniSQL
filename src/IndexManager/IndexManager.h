#ifndef INDEXMANAGER_H_
#define INDEXMANAGER_H_

#include <map>
#include <string>
#include <vector>
#include "BPTree.h"
#include "../lib/Attribute.h"

#define  D  100

class IndexManager {
public:
	using IntMap = std::map<std::string, BPTree<int, D> *>;
	using FloatMap = std::map<std::string, BPTree<float, D> *>;
	using CharMap = std::map<std::string, BPTree<std::string, D> *>;
	
	const static int TYPE_FLOAT = -2;
	const static int TYPE_INT = -1;
	// others TYPE_CHAR

	IndexManager();
	virtual ~IndexManager();
	
	// true/flase -> successful/fail
	bool create(const std::string& fileName, attr_type type);
	bool drop(const std::string& fileName, attr_type type);
	bool insert(const std::string& fileName, const Value& v, int offset);
	bool erase(const std::string& fileName, const Value& v);

	//  -1/offset -> successful/fail
	int search(const std::string& fileName, const Value& v);
	
private:
	// rum-time memory index!
	IntMap intMap;
	FloatMap floatMap;
	CharMap  charMap;

};

#endif