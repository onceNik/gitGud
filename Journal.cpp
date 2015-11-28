#include "Journal.h"

using namespace std;

Journal::Journal(uint32_t columns) {
	
	rowSize = 2;
	columnSize = columns;
	journal = new uint64_t*[rowSize];
	for(uint64_t i = 0; i < rowSize; i++) {
		journal[i] = new uint64_t[columnSize];
	}
	lastInsert = -1;
	
}

Journal::~Journal() {
	
	for(uint64_t i = 0; i < rowSize; i++) {
		delete[] journal[i];
	}
	delete[] journal;
	
}

bool Journal::insertJournalRecord(const TransactionOperationInsert_t* o, uint64_t id) {
	
	uint32_t i,j,k;
	uint64_t l;
	journal[lastInsert+1][0] = id;
	k = 0;
	for (i = 0 ; i < o->rowCount ; i++) {
		for (j = 1 ; j < columnSize ; j++) {
			journal[lastInsert+1][j] = o->values[k];
			k++;
		}
		lastInsert++;
		if (lastInsert == rowSize-1) increaseJournal();
	}
	cout << endl;
	for (l = 0 ; l <= lastInsert ; l++) {
		cout << journal[l][0] << ": ";
		for (j = 1 ; j < columnSize ; j++) {
			cout << journal[l][j] << " ";
		}
		cout << endl;
	}
	return true;
	
}

bool Journal::insertJournalRecord(const TransactionOperationDelete_t* o, uint64_t id) {
	
	uint32_t i,k;
	uint64_t j, l;
	
	for (i = 0 ; i < o->rowCount ; i++) {
		for (j = lastInsert ; j >= 0 ; j--) {
			if (journal[j][1] == o->keys[i]) break;
		}
		journal[lastInsert+1][0] = id;
		for (k = 1 ; k < columnSize ; k++) {
			journal[lastInsert+1][k] = journal[j][k];
		}
	}
	lastInsert++;
	if (lastInsert == rowSize-1) increaseJournal();
	cout << endl;
	for (l = 0 ; l <= lastInsert ; l++) {
		cout << journal[l][0] << ": ";
		for (j = 1 ; j < columnSize ; j++) {
			cout << journal[l][j] << " ";
		}
		cout << endl;
	}
	return true;
	
}

bool Journal::increaseJournal() {
	
	uint64_t newSize;
	newSize = 2*rowSize;
	uint64_t** newJournal = new uint64_t*[newSize];
	for (uint64_t i = 0 ; i < newSize; i++) {
		newJournal[i] = new uint64_t[columnSize];
	}
	
	for (i = 0; i < rowSize; i++) {
		for(uint32_t j = 0; j < columnSize; j++) {
			newJournal[i][j] = journal[i][j];
		}
	}
	
	for(i = 0 ; i < rowSize; i++) {
		delete[] journal[i];
	}
	delete[] journal;
	rowSize = newSize;
	journal = newJournal;
	cout << "increased " << rowSize << endl;
	
	return true;
	
}

/*
bool Journal::deleteJournalRecord() {}

List<kati> Journal::getJournalRecords() {}

bool Journal::destroyJournal() {}
*/


