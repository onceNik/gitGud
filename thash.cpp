#include <iostream>
#include <cstdlib>
#include <cmath>
#include "thash.h"

using namespace std;

T_Entry::T_Entry(uint64_t id, uint64_t off) {
	tid = id;
	offset = off;
}

T_Entry::~T_Entry() {}

uint64_t T_Entry::get_tid() {
	return tid;
}

uint64_t T_Entry::get_offset() {
	return offset;
}

void T_Entry::printentry() {
	cout << tid << " " << offset << endl;	
}

//////////////////////////////////////////////////

T_Bucket::T_Bucket(int ld) {
	
	eSize = 2;
	total = 0;
	entryTable = (T_Entry**)malloc(sizeof(T_Entry)*eSize);
	for (uint64_t i = 0 ; i < eSize ; i++) {
		entryTable[i] = NULL;
	}
	localDepth = ld;
	
}

T_Bucket::~T_Bucket() {
	
	for (uint64_t i = 0 ; i < eSize ; i++) {
		delete entryTable[i];
	}
	free(entryTable);
	
}

bool T_Bucket::addToBucket(uint64_t** journal, uint64_t id, uint64_t off, bool* abort) {
	
	bool succ = false;
	for (uint64_t i = 0 ; i < eSize ; i++) {
		if(entryTable[i] == NULL) {
			entryTable[i] = new T_Entry(id,off);
			succ = true;
			break;
		}
		else {
			if(id == entryTable[i]->get_tid()) {
				*abort = true;
				break;
			}			
		}
	}
	return succ;
	
}

bool T_Bucket::full() {
	
	if (total == eSize) return true;
	else return false;
	
}

int T_Bucket::get_localDepth() {
	return localDepth;
}

int T_Bucket::get_eSize() {
	return eSize;
}

void T_Bucket::printbucket() {
	for (int i = 0 ; i < eSize ; i++) {
		if (entryTable[i] != NULL) entryTable[i]->printentry();
	}
}

T_Entry** T_Bucket::get_table() {
	return entryTable;
}

//////////////////////////////////////////////////

T_hashMap::T_hashMap() {
	
	T_Bucket* b;
	hSize = 2;
	hMap = (T_Bucket**)malloc(sizeof(T_Bucket)*hSize);
	if (hMap == NULL) {
		cout << "Failed to allocate memory for hash map" << endl;
		exit(1);
	}
	b = new T_Bucket(0);
	for (int i = 0 ; i < hSize ; i++) hMap[i] = b;
	globalDepth = 1;
	
}

T_hashMap::~T_hashMap() {

	for (int i = 0 ; i < hSize ; i++) {
		if (hMap[i] != NULL) delete hMap[i];
	}
	free(hMap);
	
}

void T_hashMap::insertHashRecord(uint64_t** journal, uint64_t id, uint64_t off) {
	
	bool abort = false;
	T_Bucket* dlt;
	uint64_t bit;
	
	bit = id & ((1 << globalDepth)-1);
	dlt = hMap[bit];
	bool succ = hMap[bit]->addToBucket(journal,id,off,&abort);
	
	if (abort) { cout << "ALREADY EXISTS" << endl; return; }
	if (!succ && (hMap[bit]->get_localDepth() == globalDepth)) {
		//cout << "DOUBLE" << endl;
		doubleMap();
		globalDepth++;
	}
	if (!succ && (hMap[bit]->get_localDepth() < globalDepth)) {
		//cout << endl << "SPLIT" << endl;
		uint64_t temp;
		uint64_t h;
		T_Bucket* b1;
		T_Bucket* b2;
		b1 = new T_Bucket(hMap[bit]->get_localDepth()+1);
		b2 = new T_Bucket(hMap[bit]->get_localDepth()+1);
		for (int i = 0 ; i < hMap[bit]->get_eSize() ; i++) {
			temp = ((hMap[bit]->get_table())[i])->get_tid();
			h = (temp)&((1 << globalDepth)-1);
			//cout << "H IS " << h << endl;
			if (((h >> hMap[bit]->get_localDepth())&1) == 0) {
				//cout << endl << "STELNO: " << hMap[bit]->get_table()[i]->get_tid() << " " << journal[temp[0]][1] << " " << temp[0] << endl;
				b1->addToBucket(journal,(hMap[bit]->get_table())[i]->get_tid(),(hMap[bit]->get_table())[i]->get_offset(),&abort);
				//cout << "sto 0" << endl;
			}
			else {
				//cout << endl << "STELNO: " << hMap[bit]->get_table()[i]->get_tid() << " " << journal[temp[0]][1] << " " << temp[0] << endl;
				b2->addToBucket(journal,(hMap[bit]->get_table())[i]->get_tid(),(hMap[bit]->get_table())[i]->get_offset(),&abort);
				//cout << "sto 1" << endl;
			}
		}
		for (int i = 0 ; i < hSize ; i++) {
			//cout << "III: " << i << endl;
			//hMap[i]->printbucket();
			//dlt->printbucket();
			if (hMap[i] == dlt) {
				//cout << "einai" << endl; 
				if (((i >> dlt->get_localDepth())&1) == 0) {
					//cout << "poses fores?" << endl;
					hMap[i] = b1;
					//cout << "test" << endl;
					//hMap[i]->printbucket();
				}
				else {
					//cout << "5 fores?" << endl;
					hMap[i] = b2;
					//cout << "test2" << endl;
					//hMap[i]->printbucket();
				}
			}
		}
		bit = id & ((1 << globalDepth)-1);
		if (((bit >> dlt->get_localDepth())&1) == 0){
			 if (!(b1->addToBucket(journal,id,off,&abort))) {
				insertHashRecord(journal,id,off);
			 }
			 //cout << "b1" << endl;
		 }
		else {
			if (!(b2->addToBucket(journal,id,off,&abort))) {
				insertHashRecord(journal,id,off);
			}
			//cout << "b2" << endl;
		}
		delete dlt;
	}
	
}

void T_hashMap::doubleMap() {
	
	T_Bucket** newMap = new T_Bucket*[2*hSize];
	for (int i = 0 ; i < hSize ; i++) newMap[i] = hMap[i];
	for (int i = hSize ; i < 2*hSize ; i++) {
		uint64_t bit = i & ((1 << globalDepth)-1);
		newMap[i] = hMap[bit];
	}
	for (int i = 0 ; i < hSize ; i++) hMap[i] = NULL;
	free(hMap);
	hMap = newMap;
	hSize = 2*hSize;
	
}

void T_hashMap::printhash() {
	cout << endl << endl;
	for (int i = 0 ; i < hSize ; i++) {
		cout << i << ": ";
		hMap[i]->printbucket();
	}	
}

T_Bucket* T_hashMap::getHashRecords(uint64_t key) {
	return hMap[key & ((1 << globalDepth)-1)];
}
