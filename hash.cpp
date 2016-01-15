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

void Bucket::addToBucket(uint64_t id, uint64_t off, bool mode) {
	
	for (uint64_t i = 0 ; i < eSize ; i++) {
		if (entryTable[i] == NULL) {
			entryTable[i] = new Entry(id,off,mode);
			total++;
			break;
		}
		else if (id == entryTable[i]->get_tid()) {
			updateEntry(id,off,mode);
			break;
		}
	}
	
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
		entryTable[i]->printentry();
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
	
	uint64_t bit;
	
	bit = key & ((1 << globalDepth)-1);
	if (hMap[bit]->full() && (hMap[bit]->get_localDepth() == globalDepth)) {
		doubleMap();
		globalDepth++;
	}
	if (hMap[bit]->full() && (hMap[bit]->get_localDepth() < globalDepth)) {
		int temp[2];
		uint64_t h;
		Bucket* b1;
		Bucket* b2;
		b1 = new Bucket(hMap[bit]->get_localDepth()+1);
		b2 = new Bucket(hMap[bit]->get_localDepth()+1);
		for (int i = 0 ; i < hMap[bit]->get_eSize() ; i++) {
			temp = ((hMap[bit]->get_table())[i])->get_offset();
			if (temp[0] != -1) {
				h = (journal[temp[0]][1])&((1 << globalDepth)-1);
				if ((h >> hMap[bit]->get_localDepth())&1 == 0) b1->addToBucket((hMap[bit]->get_table())[i]->get_tid(),temp[0],0);
				else b2->addToBucket((hMap[bit]->get_table())[i]->get_tid(),temp[0],0);
			}
			if (temp[1] != -1) {
				h = (journal[temp[1]][1])&((1 << globalDepth)-1);
				if ((h >> hMap[bit]->get_localDepth())&1 == 0) b1->addToBucket((hMap[bit]->get_table())[i]->get_tid(),temp[0],0);
				else b2->addToBucket((hMap[bit]->get_table())[i]->get_tid(),temp[0],0);
			}
		}
		if ((bit >> hMap[bit]->get_localDepth())&1 == 0) b1->addToBucket(id,off,mode);
		else b2->addToBucket(id,off,mode);
		for (int i = 0 ; i < hSize ; i++) {
			if (hMap[i] == hMap[bit]) {
				if ((i >> hMap[bit]->get_localDepth())&1) == 0) hMap[i] = b1;
				else hMap[i] = b2;
			}
		}
	}
	else hMap[bit]->addToBucket(id,off,mode);
	
}

void hashMap::doubleMap() {
	
	Bucket** newMap = new Bucket*[2*hSize];
	for (int i = 0 ; i < hSize ; i++) newMap[i] = hMap[i];
	for (int i = hSize ; i < 2*hSize ; i++) {
		bit = i & ((1 << globalDepth-1)-1);
		newMap[i] = hMap[bit];
	}
	for (int i = 0 ; i < hSize ; i++) hMap[i] = NULL;
	free(hMap);
	hMap = newMap;
	hSize = 2*hSize;
	
}

void hashMap::printhash() {
	for (int i = 0 ; i < hSize ; i++) {
		cout << i << ": ";
		hMap[i]->printbucket();
	}	
}
