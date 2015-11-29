#include "list.h"

List::List() {
	listHead = NULL;
}

List::~List() {
	listItem* temp;
	while (listHead != NULL) {
		temp = listHead;
		listHead = temp->next;
		delete temp->ptr;
		delete temp;
	}
}

void List::push(uint64_t cs, uint64_t* r) {

	listItem* t;
	t = new listItem;
	t->ptr = new uint64_t[cs];
	for (uint64_t i = 0 ; i < cs ; i++) {
		t->ptr[i] = r[i];
	}
	t->next = NULL;
	if (listHead == NULL) listHead = t;

}

listItem* List::get_listHead() {
	return listHead;
}
