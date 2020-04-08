#include <iostream>
#include "../Interpreter/Interpreter.h"
#include "../CatalogManager/CatalogManager.h"
#include "../RecordManager/RecordManager.h"
#include "../IndexManager/IndexManager.h"
#include "../API/API.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

API api;
void print_time(clock_t start);

int main(int argc, char * argv[]) {
	CatalogManager cm;
	RecordManager rm;
	api.rm = &rm;
	api.cm = &cm;
	IndexManager im;
	api.im = &im;
	rm.api = &api;
	Interpreter in;
	in.ap = &api;

	clock_t start;
	bool fileRead = false;
	ifstream file;
	string s;
	int status = 0;
	while(1) {
		if(status == STATUS_CONTINUE)
			continue;
		if(fileRead) {
			file.open(in.fileName.c_str());
			if(!file.is_open()) {
				cout << "Fail to open " << in.fileName << endl;
				fileRead = false;
				continue;
			}
			while(getline(file, s, ';')) {
				in.interprete(s);
			}
			file.close();
			fileRead = false;
			print_time(start);
		} else {
			cout << "minisql>>";
			getline(cin, s, ';');
			start = clock();
			status = in.interprete(s);
			if(status == STATUS_FILEREAD) {
				fileRead = true;
				continue;
			}
			if(status == STATUS_QUIT) {
				break;
			}
			print_time(start);
		}
	}
	return 0;
}

void print_time(clock_t start) {
	clock_t finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	duration *= 1000;
	printf("%2.1f milliseconds\n", duration);
}