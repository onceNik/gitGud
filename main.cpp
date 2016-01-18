#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <inttypes.h>
#include <cstdint>
#include "Journal.h"

using namespace std;

static uint32_t* schema = NULL;
static uint32_t schemaSize = 0;

static void processDefineSchema(DefineSchema_t *s) {

	int i;
	printf("DefineSchema %d |", s->relationCount);
	if (schema == NULL) free(schema);
	schema = (uint32_t*)malloc(sizeof(uint32_t)*s->relationCount);
	schemaSize = s->relationCount;

	for (i = 0 ; i < s->relationCount ; i++) {
		printf(" %d ",s->columnCounts[i]);
		schema[i] = s->columnCounts[i];
	}
	printf("\n");

}

static void processTransaction(Journal** jTable, Transaction_t *t){
	
	int i;
	const char* reader = t->operations;
	printf("Transaction %llu (%u, %u) |", t->transactionId, t->deleteCount, t->insertCount);
	for (i = 0 ; i < t->deleteCount ; i++) {
		const TransactionOperationDelete_t* o = (TransactionOperationDelete_t*)reader;
		jTable[o->relationId]->insertJournalRecord(o,t->transactionId);
		printf("opdel rid %u #rows %u ", o->relationId, o->rowCount);
		reader+=sizeof(TransactionOperationDelete_t)+(sizeof(uint64_t)*o->rowCount);
	}
	printf(" \t| ");
	for(i = 0 ; i < t->insertCount ; i++) {
		const TransactionOperationInsert_t* o = (TransactionOperationInsert_t*)reader;
		jTable[o->relationId]->insertJournalRecord(o,t->transactionId);
		printf("opins rid %u #rows %u |", o->relationId, o->rowCount);
		reader+=sizeof(TransactionOperationInsert_t)+(sizeof(uint64_t)*o->rowCount*schema[o->relationId]);
	}
	printf("\n");

}

static void processValidationQueries(vList* l,ValidationQueries_t *v){
	
	printf("ValidationQueries %llu [%llu, %llu] %u\n", v->validationId, v->from, v->to, v->queryCount);
	l->push(v);
	/*const char* reader = v->queries;
	for (int i = 0 ; i < v->queryCount ; i++) {
		const Query_t* o = (Query_t*)reader;
		printf("Val: relId %u colCount %u \n", o->relationId, o->columnCount);
		for (int j = 0 ; j < o->columnCount ; j++) {
			printf("\n%u , %d, %llu\n\n", (o->columns[j]).column, (o->columns[j]).op, (o->columns[j]).value);
		} 
		reader+=sizeof(Query_t)+(sizeof(Column_t)*(o->columnCount));
	}*/
}

static void processFlush(Flush_t *fl){
	printf("Flush %llu\n", fl->validationId);
}

static void processForget(Forget_t *fo){
	printf("Forget %llu\n", fo->transactionId);
}

int main(int argc, char **argv) {

	MessageHead_t head;
	void *body = NULL;
	uint32_t len;
	Journal** jTable;
	tList* l;
	vList* v;
	tListItem* temp;
	vListItem* tmp;

	v = new vList();
	while(1){
		
		if (read(0, &head, sizeof(head)) <= 0) { return -1; }
		printf("HEAD LEN %u \t| HEAD TYPE %u \t| DESC ", head.messageLen, head.type);


		if (body != NULL) free(body);
		if (head.messageLen > 0 ){
			body = malloc(head.messageLen*sizeof(char));
			if (read(0, body, head.messageLen) <= 0) { printf("err");return -1; } // crude error handling, should never happen
			len-=(sizeof(head) + head.messageLen);
		}

		switch (head.type) {
			case Done:
				jTable[0]->printhash();
				l = jTable[0]->getJournalRecords(0,3);
				temp = l->get_tListHead();
				while (temp != NULL) {
					for (uint32_t i = 0 ; i < jTable[0]->get_columnSize() ; i++) {
						cout << temp->ptr[i];
					}
					cout << endl;
					temp = temp->next;
				}
				for (int i = 0 ; i < schemaSize ; i++) {
					delete jTable[i];
				}
				delete jTable;
				printf("\n");
				
				/////////////////////////////////////////////
				
				v->printlist();
				
				return 0;
			case DefineSchema: 
				processDefineSchema((DefineSchema_t*)body);
				jTable = new Journal*[schemaSize];
				for (int i = 0 ; i < schemaSize ; i++) {
					jTable[i] = new Journal(schema[i]+1);
					printf("%d\n",schema[i]);
				}
				break;
			case Transaction: 
				processTransaction(jTable,(Transaction_t*)body); 
				break;
			case ValidationQueries: 
				processValidationQueries(v,(ValidationQueries_t*)body); 
				break;
			case Flush: processFlush((Flush_t*)body); break;
			case Forget: processForget((Forget_t*)body); break;
			default:
			return -1;
		}
	}
	return 0;
	
}
