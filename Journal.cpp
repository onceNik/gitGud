#include "Journal.h"

using namespace std;


Journal::Journal(uint32_t columns) {
	columnSize = columns;
	createJournal(columnSize);
	
	}

Journal::~Journal() {}

Journal* Journal::createJournal(uint32_t columnSize) {
	
	journal = new uint64_t*[rowSize];
	for(uint64_t i = 0; i < rowSize; i++) {
		journal[i] = new uint64_t[columnSize];
	}
	
	return j
	
}

bool Journal::insertJournalRecord() {}

bool Journal::deleteJournalRecord() {}

List<kati> Journal::getJournalRecords() {}

bool Journal::destroyJournal() {}

bool Journal::increaseJournal() {										/* ΦΤΙΑΞΕ ΕΛΕΓΧΟΥΣ ΓΙΑ ΜΝΗΜΗ */
	uint64_t newSize;
	
	newSize = 2 * rowSize;
	uint64_t** newJournal = new uint64_t*[newSize];
	for(uint64_t i = 0; i < newSize; i++) {
		newJournal[i] = new uint64_t[columnSize];
	}
	
	for(uint64_t i= 0; i < rowSize; i++) {
		for(uint64_t j = 0; j < columnSize; j++) {
			newJournal[i][j] = journal[i][j];
		}
	}
	
	for(uint64_t i= 0; i < rowSize; i++) {
		delete []journal[i];
	}
	delete [] journal;
	
	journal = newJournal;
	
	return true;
}


