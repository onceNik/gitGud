#ifndef THASH_H
#define THASH_H

class T_Entry {
	private:
		uint64_t tid;
		uint64_t offset;	
	public:
		T_Entry(uint64_t,uint64_t);
		~T_Entry();
		uint64_t get_tid();
		uint64_t get_offset();
		void printentry();
};

//////////////////////////////////////////////////

class T_Bucket {
	private:
		int localDepth;
		T_Entry** entryTable;
		uint64_t eSize;
		uint64_t total;
	public:
		T_Bucket(int);
		~T_Bucket();
		bool addToBucket(uint64_t**,uint64_t,uint64_t,bool*);
		bool full();
		int get_localDepth();
		int get_eSize();
		void printbucket();
		T_Entry** get_table();
};

//////////////////////////////////////////////////

class T_hashMap {
	private:
		T_Bucket** hMap;
		int globalDepth;
		int hSize;
	public:
		T_hashMap();
		~T_hashMap();
		void insertHashRecord(uint64_t**,uint64_t,uint64_t);
		void doubleMap();
		void printhash();
		T_Bucket* getHashRecords(uint64_t);
};

#endif
