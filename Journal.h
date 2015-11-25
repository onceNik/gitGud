#include <iostream>
#include <cstdlib>

class Journal {
	
	private:
		uint64_t rowSize = 2000;
		uint32_t columnSize;
		uint64_t** journal;
	
	public:
		Journal(uint32_t);
		~Journal();
		Journal* createJournal(uint32_t);														/* Journal constructor - args pending */
		bool insertJournalRecord(Journal* , TransactionOperationInsert_t*, +);                		/* inserts Record in Journal, returns success/fail - args pending */
		bool deleteJournalRecord(Journal* , TransactionOperationDelete_t*, +);                		/* inserts Record in Journal, returns success/fail - args pending */
		List<Record*> getJournalRecords(Journal* , JournalRecord* , int start, int end, +++);	/* returns a pointer to a list of Records from start to end - args pending */
		bool destroyJournal(Journal*);													/* free allocated space for specific Journal, returns success/fail */
		bool increaseJournal(Journal*, +++);											/* increase size of Journal (prob double), returns success/fail - args pending */

};
