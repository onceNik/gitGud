#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <cstdint>
#include "structs.h"

typedef struct tListItem {
	uint64_t* ptr;
	struct tListItem* next;
}tListItem;

typedef struct vListItem {
	ValidationQueries_t* ptr;
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
