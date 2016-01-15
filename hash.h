#ifndef HASH_H
#define HASH_H

class Entry {
	private:
		uint64_t tid;
		uint64_t offset[2];	
	public:
		Entry(uint64_t, uint64_t,bool);
		~Entry();
		void updateEntry(uint64_t,bool);
		uint64_t get_tid();
		uint64_t* get_offset();
		void printentry();
};

//////////////////////////////////////////////////

class Bucket {
	private:
		int localDepth;
		Entry** entryTable;
		uint64_t eSize;
		uint64_t total;
	public:
		Bucket(int);
		~Bucket();
		bool addToBucket(uint64_t**,uint64_t,uint64_t,uint64_t,bool);
		bool full();
		int get_localDepth();
		int get_eSize();
		Entry** get_table();
		void printbucket();
};

//////////////////////////////////////////////////

class hashMap {
	private:
		Bucket** hMap;
		int globalDepth;
		int hSize;
	public:
		hashMap();
		~hashMap();
		void insertHashRecord(uint64_t**,uint64_t,uint64_t,uint64_t,bool);
		void doubleMap();
		void printhash();
};

#endif
