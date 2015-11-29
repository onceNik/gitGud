#include <iostream>
#include <cstdlib>
#include <cmath>
#include "hash.h"

using namespace std;

Bucket::Bucket(uint64_t key, int ld) {

	offset[0] = -1;
	offset[1] = -1;
	localDepth = ld;
	pk = key;
	
}

Bucket::~Bucket() {}

bool Bucket::addtoBucket(unsigned long id, Record* rec) {
	
	bool succ = false;
	int i;
	for (i = 0 ; i < bucketEntries ; i++) {
		if (table[i] == NULL) {
			table[i] = new Entry(id,rec);
			succ = true;
			break;
		}
	}
	if (succ) return succ;
	if (next != NULL) {
		succ = next->addtoBucket(id,rec);
		return succ;
	}
	else return succ;
	
}

bool Bucket::relocateEntry(Entry* e) {
	
	bool succ = false;
	int i;
	for (i = 0 ; i < bucketEntries ; i++) {
		if (table[i] == NULL) {
			table[i] = e;
			succ = true;
			break;
		}
	}
	if (succ) return succ;
	if (next != NULL) {
		succ = next->relocateEntry(e);
		return succ;
	}
	else return succ;
	
}

void Bucket::split(Bucket** map, int p, int sRound, int m) {
	
	int i;
	int index, succ;
	for (i = 0 ; i < bucketEntries ; i++) {
		if (table[i] == NULL) continue;
		index = (table[i]->get_id())%((int)pow(2,sRound+1)*m);
		if (index != p) {
			succ = map[index]->relocateEntry(table[i]);
			if (!succ) map[index]->extendAndLink(table[i]);
			table[i] = NULL;
		}
	}
	if (next != NULL) next->split(map,p,sRound,m);
	
}

//////////////////////////////////////////////////

hashMap::hashMap() {
	
	Bucket* b;
	size = 2;
	map = (Bucket**)malloc(sizeof(Bucket)*size);
	if (map == NULL) {
		cout << "Failed to allocate memory for hash map" << endl;
		exit(1);
	}
	b = new Bucket(-1,0);
	for (int i = 0 ; i < size ; i++) map[i] = b;
	globalDepth = 1;
	
}

hashMap::~hashMap() {

	for (int i = 0 ; i < size ; i++) {
		if (map[i] != NULL) delete map[i];
	}
	free(map);
	
}

void hashMap::insertHashRecord(uint64_t key) {
	
	int index;
	bool succ;
	index = i%((int)pow(2,sRound)*m);
	if (index < p) index = i%((int)pow(2,sRound+1)*m);
	
	succ = map[index]->addtoBucket(i,rec);
	if (!succ) map[index]->addtoOverflow(i,rec);
	total++;
	
}
