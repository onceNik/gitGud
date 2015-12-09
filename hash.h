#ifndef HASH_H
#define HASH_H


class Entry {
	private:
		uint64_t tid;
		uint64_t offset[2];	
	public:
		Entry(uint64_t, uint64_t,bool);
		~Entry();
		updateEntry(uint64_t,bool);
		uint64_t get_tid();
	
};


//////////////////////////////////////////////////

class Bucket {
	private:
		int localDepth;
		Entry** entryTable;
		uint64_t eSize;
		uint64_t pkey;
	public:
		Bucket(int);
		~Bucket();
		bool addToBucket(uint64_t,uint64_t,bool);
		uint64_t get_pkey();
		int get_localDepth();
		void set_pkey(uint64_t);
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
		void insertHashRecord(uint64_t,uint64_t,uint64_t,bool);
		void doubleMap(uint64_t,uint64_t,uint64_t,bool)
};

#endif
