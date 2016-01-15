#include <iostream>
#include <cstdlib>
#include <cmath>
#include "hash.h"

using namespace std;

Entry::Entry(uint64_t id, uint64_t off, bool mode) {
	
	tid = id;
	offset[0] = -1;
	offset[1] = -1;
	offset[mode] = off;
	
}

Entry::~Entry() {}

void Entry::updateEntry(uint64_t off, bool mode) {
	offset[mode] = off;
}

uint64_t Entry::get_tid() {
	return tid;
}

uint64_t* Entry::get_offset() {
	return offset;
}

void Entry::printentry() {
	cout << tid << " " << offset[0] << " " << offset[1] << endl;	
}

///////////////////////////////////////////////////////

Bucket::Bucket(int ld) {
	
	eSize = 5;
	total = 0;
	entryTable = (Entry**)malloc(sizeof(Entry)*eSize);
	for (uint64_t i = 0 ; i < eSize ; i++) {
		entryTable[i] = NULL;
	}
	localDepth = ld;
	
}

Bucket::~Bucket() {
	
	for (uint64_t i = 0 ; i < eSize ; i++) {
		delete entryTable[i];
	}
	free(entryTable);
	
}

bool Bucket::addToBucket(uint64_t** journal, uint64_t id, uint64_t key, uint64_t off, bool mode) {
	
	bool succ = false;
	for (uint64_t i = 0 ; i < eSize ; i++) {
		if (entryTable[i] == NULL) {
			entryTable[i] = new Entry(id,off,mode);
			succ = true;
			break;
		}
		else if (id == entryTable[i]->get_tid()) {
			uint64_t o1, o2;
			uint64_t k;
			o1 = (entryTable[i]->get_offset())[0];
			o2 = (entryTable[i]->get_offset())[1];
			if (o1 != -1 && o2 != -1) continue;
			if (o1 != -1) k = journal[o1][1];
			else k = journal[o2][1];
			if (k != key) continue;
			else {
				entryTable[i]->updateEntry(off,mode);
				succ = true;
				break;
			}					
		}
	}
	return succ;
	
}

bool Bucket::full() {
	
	if (total == eSize) return true;
	else return false;
	
}

int Bucket::get_localDepth() {
	return localDepth;
}

int Bucket::get_eSize() {
	return eSize;
}

Entry** Bucket::get_table() {
	return entryTable;
}

void Bucket::printbucket() {
	
	for (int i = 0 ; i < eSize ; i++) {
		if (entryTable[i] != NULL) entryTable[i]->printentry();
	}
	
}

//////////////////////////////////////////////////

hashMap::hashMap() {
	
	Bucket* b;
	hSize = 2;
	hMap = (Bucket**)malloc(sizeof(Bucket)*hSize);
	if (hMap == NULL) {
		cout << "Failed to allocate memory for hash map" << endl;
		exit(1);
	}
	b = new Bucket(0);
	for (int i = 0 ; i < hSize ; i++) hMap[i] = b;
	globalDepth = 1;
	
}

hashMap::~hashMap() {

	for (int i = 0 ; i < hSize ; i++) {
		if (hMap[i] != NULL) delete hMap[i];
	}
	free(hMap);
	
}

void hashMap::insertHashRecord(uint64_t** journal, uint64_t id, uint64_t key, uint64_t off, bool mode) {
	
	Bucket* dlt;
	uint64_t bit;
	bit = key & ((1 << globalDepth)-1);
	dlt = hMap[bit];
	bool succ = hMap[bit]->addToBucket(journal,id,key,off,mode);
	if (!succ && (hMap[bit]->get_localDepth() == globalDepth)) {
		cout << "DOUBLE" << endl;
		doubleMap();
		globalDepth++;
	}
	if (!succ && (hMap[bit]->get_localDepth() < globalDepth)) {
		cout << endl << "SPLIT" << endl;
		uint64_t* temp;
		uint64_t h;
		Bucket* b1;
		Bucket* b2;
		b1 = new Bucket(hMap[bit]->get_localDepth()+1);
		b2 = new Bucket(hMap[bit]->get_localDepth()+1);
		for (int i = 0 ; i < hMap[bit]->get_eSize() ; i++) {
			temp = ((hMap[bit]->get_table())[i])->get_offset();
			if (temp[0] != -1) {
				h = (journal[temp[0]][1])&((1 << globalDepth)-1);
				cout << "H IS " << h << endl;
				if (((h >> hMap[bit]->get_localDepth())&1) == 0) {
					cout << endl << "STELNO: " << hMap[bit]->get_table()[i]->get_tid() << " " << journal[temp[0]][1] << " " << temp[0] << endl;
					b1->addToBucket(journal,(hMap[bit]->get_table())[i]->get_tid(),journal[temp[0]][1],temp[0],0);
					cout << "sto 0" << endl;
				}
				else {
					cout << endl << "STELNO: " << hMap[bit]->get_table()[i]->get_tid() << " " << journal[temp[0]][1] << " " << temp[0] << endl;
					b2->addToBucket(journal,(hMap[bit]->get_table())[i]->get_tid(),journal[temp[0]][1],temp[0],0);
					cout << "sto 1" << endl;
				}
			}
			if (temp[1] != -1) {
				cout << "bika" << endl;
				h = (journal[temp[1]][1])&((1 << globalDepth)-1);
				if (((h >> hMap[bit]->get_localDepth())&1) == 0) {
					 cout << endl << "STELNO: " << hMap[bit]->get_table()[i]->get_tid() << " " << journal[temp[1]][1] << " " << temp[1] << endl;
					 b1->addToBucket(journal,(hMap[bit]->get_table())[i]->get_tid(),journal[temp[1]][1],temp[1],1);
					 cout << "sto 0" << endl;
				 }
				else { 
					cout << endl << "STELNO: " << hMap[bit]->get_table()[i]->get_tid() << " " << journal[temp[1]][1] << " " << temp[1] << endl;
					b2->addToBucket(journal,(hMap[bit]->get_table())[i]->get_tid(),journal[temp[1]][1],temp[1],1);
					cout << "sto 1" << endl;
				}
			}
		}
		if (((bit >> hMap[bit]->get_localDepth())&1) == 0){
			 b1->addToBucket(journal,id,key,off,mode);
			 cout << "b1" << endl;
		 }
		else {
			cout << "b2" << endl;
			b2->addToBucket(journal,id,key,off,mode);
		}
		for (int i = 0 ; i < hSize ; i++) {
			cout << "III: " << i << endl;
			hMap[i]->printbucket();
			dlt->printbucket();
			if (hMap[i] == dlt) {
				cout << "einai" << endl; 
				if (((i >> dlt->get_localDepth())&1) == 0) {
					cout << "poses fores?" << endl;
					hMap[i] = b1;
					cout << "test" << endl;
					hMap[i]->printbucket();
				}
				else {
					cout << "5 fores?" << endl;
					hMap[i] = b2;
					cout << "test2" << endl;
					hMap[i]->printbucket();
				}
			}
		}
		delete dlt;
	}
	printhash();
	
}

void hashMap::doubleMap() {
	
	Bucket** newMap = new Bucket*[2*hSize];
	for (int i = 0 ; i < hSize ; i++) newMap[i] = hMap[i];
	for (int i = hSize ; i < 2*hSize ; i++) {
		uint64_t bit = i & ((1 << globalDepth-1)-1);
		newMap[i] = hMap[bit];
	}
	for (int i = 0 ; i < hSize ; i++) hMap[i] = NULL;
	free(hMap);
	hMap = newMap;
	hSize = 2*hSize;
	
}

void hashMap::printhash() {
	cout << endl << endl;
	for (int i = 0 ; i < hSize ; i++) {
		cout << i << ": ";
		hMap[i]->printbucket();
	}	
}
