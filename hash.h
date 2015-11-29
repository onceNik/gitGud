#ifndef HASH_H
#define HASH_H

//////////////////////////////////////////////////

class Bucket {
	private:
		int offset[2];
		int localDepth;
		uint64_t pk;
	public:
		Bucket(uint64_t,int);
		~Bucket();
		bool addtoBucket(unsigned long, Record*);
		bool relocateEntry(Entry*);
		void split(Bucket**,int,int,int);
};

//////////////////////////////////////////////////

class hashMap {
	private:
		Bucket** map;
		int globalDepth;
		int size;
	public:
		hashMap();
		~hashMap();
		void insertHashRecord(uint64_t);
};

#endif
