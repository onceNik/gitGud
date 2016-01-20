#ifndef JOURNAL_H
#define JOURNAL_H

#include <iostream>
#include <cstdlib>
#include "list.h"
#include "hash.h"

class Journal {
	
	private:
		uint64_t rowSize;
		uint32_t columnSize;
		uint64_t** journal;
		uint64_t lastInsert;
		hashMap* h;
	
	public:
		Journal(uint32_t);
		~Journal();
		bool insertJournalRecord(const TransactionOperationInsert_t*,uint64_t);                		
		bool insertJournalRecord(const TransactionOperationDelete_t*,uint64_t);
		bool increaseJournal();
		void printhash();
		tList* getJournalRecords(uint64_t,uint64_t);
		uint32_t get_columnSize();
		hashMap* get_hashMap();
		uint64_t** get_journal();
		
};

#endif
