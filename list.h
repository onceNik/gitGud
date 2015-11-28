#ifndef LIST_H
#define LIST_H

typedef struct {
	uint64_t* ptr;
	listItem* next;
}listItem;

class List {
	
	private:
		listItem* listHead;
	public:
		List();
		~List();
		void push(uint64_t, uint64_t*);

}

#endif
