#include <iostream>
#include <cstdlib>
#include <cmath>
#include "hash.h"

using namespace std;

Entry::Entry(uint64_t id, uint64_t off, bool mode) {
	tid = id;
	offset[mode] = off;	
}

Entry::~Entry() {}

void Entry::updateEntry(uint64_t off, bool mode) {
	offset[mode] = off;
}

uint64_t Entry::get_tid() {
	return tid;
}

///////////////////////////////////////////////////////

Bucket::Bucket(int ld) {

	pkey = -1;
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
	
	bool succ = false;
	
	for (uint64_t i = 0 ; i < eSize ; i++) {
		if (entryTable[i] == NULL) {
			entryTable[i] = new Entry(id,off,mode);
			succ = true;
			break;
		}
		else if (id == entryTable[i]->get_tid()) {
			updateEntry(id,off,mode);
			succ = true;
			break;
		}
	}
	if (!succ) {
		entryTable = (Entry**)realloc(entryTable,sizeof(Entry)*(eSize+1));
		entryTable[eSize] = new Entry(id,off,mode);
		eSize++;
	}
	
}

uint64_t Bucket::get_pkey() {
	return pkey;
}

int Bucket::get_localDepth() {
	return localDepth;
}

void Bucket::set_pkey(uint64_t key) {
	pkey = key;
}

//////////////////////////////////////////////////

hashMap::hashMap() {
	
	Bucket* b;
	hSize = 2;
	map = (Bucket**)malloc(sizeof(Bucket)*hSize);
	if (map == NULL) {
		cout << "Failed to allocate memory for hash map" << endl;
		exit(1);
	}
	b = new Bucket(0);
	for (int i = 0 ; i < hSize ; i++) map[i] = b;
	globalDepth = 1;
	
}

hashMap::~hashMap() {

	for (int i = 0 ; i < size ; i++) {
		if (map[i] != NULL) delete map[i];
	}
	free(map);
	
}

void hashMap::insertHashRecord(uint64_t id, uint64_t key, uint64_t off, bool mode) {
	
	uint64_t bit;
	
	bit = key & ((1 << globalDepth)-1);
	if (hashMap[bit]->get_pkey() == -1 || hashMap[bit]->get_pkey() == key) {
		 hashMap[bit]->addToBucket(id,off,mode);
		 hashMap[bit]->set_pkey(key);
	 }
	else {
		if(hashMap[bit]->get_localDepth() == globalDepth) {
			doubleMap(id,key,off,mode);
		}
		else {
			splitMap(id,key,off,mode);
		}
	}
}

void hashMap::doubleMap(uint64_t id, uint64_t key, uint64_t off, bool mode) {
	
	map = (Bucket**)realloc(map,sizeof(Bucket)*(hSize*2));
	
}
