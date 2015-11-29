#ifndef LIST_H
#define LIST_H

#include <unistd.h>
#include <inttypes.h>
#include <cstdint>

typedef struct listItem {
	uint64_t* ptr;
	struct listItem* next;
}listItem;

class List {
	
	private:
		listItem* listHead;
	public:
		List();
		~List();
		void push(uint64_t, uint64_t*);
		listItem* get_listHead();

};

#endif
