#include "list.h"

tList::tList() {
	tListHead = NULL;
}

tList::~tList() {
	tListItem* temp;
	while (tListHead != NULL) {
		temp = tListHead;
		tListHead = temp->next;
		delete temp->ptr;
		delete temp;
	}
}

void tList::push(uint64_t cs, uint64_t* r) {

	tListItem* t;
	tListItem* temp;
	t = new tListItem;
	t->ptr = new uint64_t[cs];
	temp = tListHead;
	if(temp != NULL) {	
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = t;
	}	
	for (uint64_t i = 0 ; i < cs ; i++) {
		t->ptr[i] = r[i];
	}
	
	t->next = NULL;
	if (tListHead == NULL) tListHead = t;

}

tListItem* tList::get_tListHead() {
	return tListHead;
}


vList::vList() {
	vListHead = NULL;
}

vList::~vList() {
	vListItem* temp;
	while (vListHead != NULL) {
		temp = vListHead;
		vListHead = temp->next;
		delete temp->ptr;
		delete temp;
	}
}

void vList::printlist() {
	
	vListItem* tmp = vListHead;
	while (tmp != NULL) {
		printf("vid: %llu from: %llu to: %u\n",(tmp->ptr)->validationId,(tmp->ptr)->from,(tmp->ptr)->to);
		const char* reader = (tmp->ptr)->queries;
		for (int i = 0 ; i < (tmp->ptr)->queryCount ; i++) {
			const Query_t* o = (Query_t*)reader;
			printf("Val: relId %u colCount %u \n", o->relationId, o->columnCount);
			for (int j = 0 ; j < o->columnCount ; j++) {
				printf("\n%u , %d, %llu\n\n", (o->columns[j]).column, (o->columns[j]).op, (o->columns[j]).value);
			}
			reader+=sizeof(Query_t)+(sizeof(Column_t)*(o->columnCount));
		}
		printf("\n");
		tmp = tmp->next;
	}

}

void vList::push(ValidationQueries_t* v) {
	
	vListItem* t;
	vListItem* temp;
	t = new vListItem;
	t->ptr = new ValidationQueries_t;
	temp = vListHead;
	if(temp != NULL) {
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = t;
	}	
	(t->ptr)->validationId = v->validationId;
	printf("%llu %llu\n",(t->ptr)->validationId,v->validationId);
	(t->ptr)->from = v->from;
	printf("%llu %llu\n",(t->ptr)->from,v->from);
	(t->ptr)->to = v->to;
	printf("%llu %llu\n",(t->ptr)->to,v->to);
	(t->ptr)->queryCount = v->queryCount;
	printf("%llu %llu\n",(t->ptr)->queryCount,v->queryCount);
	
	const char* reader = v->queries;
	for (int i = 0 ; i < v->queryCount ; i++) {
		const Query_t* o = (Query_t*)reader;
		((t->ptr)->queries[i]).relationId = o->relationId;
		((t->ptr)->queries[i]).columnCount = o->columnCount;
		for (int j = 0 ; j < o->columnCount ; j++) {
			printf("\n%u , %d, %llu\n\n", (o->columns[j]).column, (o->columns[j]).op, (o->columns[j]).value);
		}
		reader+=sizeof(Query_t)+(sizeof(Column_t)*(o->columnCount));
	}
	
	t->next = NULL;
	if (vListHead == NULL) vListHead = t;
	
}

vListItem* vList::get_vListHead() {
	return vListHead;
}
