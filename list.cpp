#include "list.h"

tList::tList() {
	tListHead = NULL;
}

tList::~tList() {
	tListItem* temp;
	while (tListHead != NULL) {
		temp = tListHead;
		tListHead = temp->next;
		delete[] temp->ptr;
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

////////////////////////////////////////////////////////////////////////

vList::vList() {
	vListHead = NULL;
}

vList::~vList() {
	vListItem* temp;
	while (vListHead != NULL) {
		temp = vListHead;
		vListHead = temp->next;
		for (int i = 0 ; i < temp->queryCount ; i++) {
			delete[] temp->queries[i].columns;
		}
		delete[] temp->queries;
		delete temp;
	}
}

void vList::printlist() {
	
	vListItem* tmp = vListHead;
	while (tmp != NULL) {
		//printf("vid: %llu from: %llu to: %u\n",tmp->valId,tmp->from,tmp->to);
		for (int i = 0 ; i < tmp->queryCount ; i++) {
			//printf("Val: relId %u colCount %u \n", (tmp->queries)[i].relationId, (tmp->queries)[i].columnCount);
			for (int j = 0 ; j < (tmp->queries)[i].columnCount ; j++) {
				//printf("\n%u , %d, %llu\n\n", ((tmp->queries)[i]).columns[j].column, ((tmp->queries)[i]).columns[j].op, ((tmp->queries)[i]).columns[j].value);
			}
		}
		//printf("\n");
		tmp = tmp->next;
	}

}

void vList::push(ValidationQueries_t* v) {
	
	vListItem* t;
	vListItem* temp;
	t = new vListItem;
	temp = vListHead;
	if(temp != NULL) {
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = t;
	}	
	t->valId = v->validationId;
	t->from = v->from;
	t->to = v->to;
	t->queryCount = v->queryCount;
	
	t->queries = new vQuery[t->queryCount];
	const char* reader = v->queries;
	for (int i = 0 ; i < v->queryCount ; i++) {
		const Query_t* o = (Query_t*)reader;
		(t->queries[i]).relationId = o->relationId;
		(t->queries[i]).columnCount = o->columnCount;
		//printf("Val: relId %u colCount %u | %u  %u\n", o->relationId, o->columnCount, (t->queries[i]).relationId, (t->queries[i]).columnCount);
		(t->queries[i]).columns = new Column_t[o->columnCount];
		for (int j = 0 ; j < o->columnCount ; j++) {
			((t->queries[i]).columns[j]).column = (o->columns[j]).column;
			((t->queries[i]).columns[j]).op = (o->columns[j]).op;
			((t->queries[i]).columns[j]).value = (o->columns[j]).value;
			//printf("\n%u , %d, %llu\n\n", (o->columns[j]).column, (o->columns[j]).op, (o->columns[j]).value);
			//printf("\n%u , %d, %llu\n\n", ((t->queries[i]).columns[j]).column, ((t->queries[i]).columns[j]).op, ((t->queries[i]).columns[j]).value);
		}
		reader+=sizeof(Query_t)+(sizeof(Column_t)*(o->columnCount));
	}
	
	
	t->next = NULL;
	if (vListHead == NULL) vListHead = t;
	
}

vListItem* vList::get_vListHead() {
	return vListHead;
}
