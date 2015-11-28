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

		//bool deleteJournalRecord(Journal* , TransactionOperationDelete_t*, +);                		/* inserts Record in Journal, returns success/fail - args pending */
		//List<Record*> getJournalRecords(Journal* , JournalRecord* , int start, int end, +++);	/* returns a pointer to a list of Records from start to end - args pending */
		//bool destroyJournal(Journal*);													/* free allocated space for specific Journal, returns success/fail */
		//bool increaseJournal(Journal*, +++);											/* increase size of Journal (prob double), returns success/fail - args pending */

};

#endif
