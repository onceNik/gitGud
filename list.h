#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <cstdint>
#include <string.h>
#include "structs.h"

typedef struct tListItem {
	uint64_t* ptr;
	struct tListItem* next;
}tListItem;

typedef struct vQuery {
	uint32_t relationId;
	uint32_t columnCount;
	Column_t* columns;	
}vQuery;

typedef struct vListItem {
	uint64_t valId;
	uint64_t from;
	uint64_t to;
	uint32_t queryCount;
	vQuery* queries;
	struct vListItem* next;
}vListItem;

class tList {
	
	private:
		tListItem* tListHead;
	public:
		tList();
		~tList();
		void push(uint64_t, uint64_t*);
		tListItem* get_tListHead();

};

class vList {
	
	private:
		vListItem* vListHead;
	public:
		vList();
		~vList();
		void push(ValidationQueries_t*);
		void printlist();
		vListItem* get_vListHead();
		
};

#endif
