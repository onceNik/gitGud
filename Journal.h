#ifndef JOURNAL_H
#define JOURNAL_H

#include <iostream>
#include <cstdlib>
#include "structs.h"

class Journal {
	
	private:
		uint64_t rowSize;
		uint32_t columnSize;
		uint64_t** journal;
		uint64_t lastInsert;
	
	public:
		Journal(uint32_t);
		~Journal();
		bool insertJournalRecord(const TransactionOperationInsert_t*,uint64_t);                		
		bool insertJournalRecord(const TransactionOperationDelete_t*,uint64_t);
		bool increaseJournal();
		List<Record*> getJournalRecords(Journal*, JournalRecord*, int start, int end);

};

#endif
