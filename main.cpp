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
	//printf("DefineSchema %d |", s->relationCount);
	if (schema == NULL) free(schema);
	schema = (uint32_t*)malloc(sizeof(uint32_t)*s->relationCount);
	schemaSize = s->relationCount;

	for (i = 0 ; i < s->relationCount ; i++) {
		//printf(" %d ",s->columnCounts[i]);
		schema[i] = s->columnCounts[i];
	}
	//printf("\n");

}

static void processTransaction(Journal** jTable, Transaction_t *t){
	
	int i;
	const char* reader = t->operations;
	//printf("Transaction %llu (%u, %u) |", t->transactionId, t->deleteCount, t->insertCount);
	for (i = 0 ; i < t->deleteCount ; i++) {
		const TransactionOperationDelete_t* o = (TransactionOperationDelete_t*)reader;
		jTable[o->relationId]->insertJournalRecord(o,t->transactionId);
		//printf("opdel rid %u #rows %u ", o->relationId, o->rowCount);
		reader+=sizeof(TransactionOperationDelete_t)+(sizeof(uint64_t)*o->rowCount);
	}
	//printf(" \t| ");
	for(i = 0 ; i < t->insertCount ; i++) {
		const TransactionOperationInsert_t* o = (TransactionOperationInsert_t*)reader;
		jTable[o->relationId]->insertJournalRecord(o,t->transactionId);
		//printf("opins rid %u #rows %u |", o->relationId, o->rowCount);
		reader+=sizeof(TransactionOperationInsert_t)+(sizeof(uint64_t)*o->rowCount*schema[o->relationId]);
	}
	//printf("\n");

}

static void processValidationQueries(vList* l, ValidationQueries_t* v) {
	
	printf("ValidationQueries %llu [%llu, %llu] %u\n",v->validationId,v->from,v->to,v->queryCount);
	l->push(v);
	
}

static void processFlush(vList* v, Journal** jo, Flush_t *fl) {
	
	//printf("Flush %llu\n", fl->validationId);
	tList* l;
	tListItem* temp;
	vListItem* tmp;
	Bucket* b;
	
	tmp = v->get_vListHead();
	while (tmp->valId <= fl->validationId) {
		int qFlag = 0;	
		for (int i = 0 ; i < tmp->queryCount ; i++) {
			int cFlag = 0;
			l = jo[(tmp->queries)[i].relationId]->getJournalRecords(tmp->from,tmp->to);
			for (int j = 0 ; j < (tmp->queries)[i].columnCount ; j++) {
				if ((((tmp->queries)[i]).columns[j].column == 0) && (((tmp->queries)[i]).columns[j].op == 0)) {
					b = (jo[(tmp->queries)[i].relationId]->get_hashMap())->getHashRecords(((tmp->queries)[i]).columns[j].value); 
					for (uint64_t k = 0 ; k < b->get_eSize() ; k++) {
						if ((b->get_table())[k] == NULL) continue;
						if (((b->get_table())[k]->get_tid() < tmp->from) || ((b->get_table())[k]->get_tid() > tmp->to)) continue;
						else {
							uint64_t** jrn = jo[(tmp->queries)[i].relationId]->get_journal();
							uint64_t* off = (b->get_table())[k]->get_offset();
							if (off[0] != -1) {
								if (jrn[off[0]][1] == ((tmp->queries)[i]).columns[j].value) {
									cFlag++;
									break;
								}
							}
							if (off[1] != -1) {
								if (jrn[off[1]][1] == ((tmp->queries)[i]).columns[j].value) {
									cFlag++;
									break;
								}
							}						
						}
					}
				}
				else {
					temp = l->get_tListHead();
					while (temp != NULL) {
						uint64_t vl = temp->ptr[((tmp->queries)[i]).columns[j].column+1];
						switch(((tmp->queries)[i]).columns[j].op) {
							case 0:
								if (vl == ((tmp->queries)[i]).columns[j].value) cFlag++;
								break;
							case 1:
								if (vl != ((tmp->queries)[i]).columns[j].value) cFlag++;
								break;
							case 2:
								if (vl < ((tmp->queries)[i]).columns[j].value) cFlag++;
								break;
							case 3:
								if (vl <= ((tmp->queries)[i]).columns[j].value) cFlag++;
								break;
							case 4:
								if (vl > ((tmp->queries)[i]).columns[j].value) cFlag++;
								break;
							case 5:
								if (vl >= ((tmp->queries)[i]).columns[j].value) cFlag++;
								break;
							default:
								printf("Error\n");
								break;
						}								
						temp = temp->next;
					}	
				}
			}
			delete l;
			if ((cFlag == (tmp->queries)[i].columnCount) && ((tmp->queries)[i].columnCount != 0)) {
				qFlag = 1;
				break;
			}
		}		
		//printf("valId: %d RESULT: %d\n",tmp->valId,qFlag);
		tmp = tmp->next;
		if (tmp == NULL) break;
	}
	cout << endl;
	
}

static void processForget(Forget_t *fo) {
	//printf("Forget %llu\n", fo->transactionId);
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
		//printf("HEAD LEN %u \t| HEAD TYPE %u \t| DESC ", head.messageLen, head.type);


		if (body != NULL) free(body);
		if (head.messageLen > 0 ){
			body = malloc(head.messageLen*sizeof(char));
			if (read(0, body, head.messageLen) <= 0) { 
				printf("err");
				return -1; 
			}
			len-=(sizeof(head) + head.messageLen);
		}

		switch (head.type) {
			case Done:
				tList* t1;
				tList* t2;				
				t1 = jTable[0]->getJournalRecords(0,10);
				t2 = jTable[1]->getJournalRecords(0,10);
				t1->printlist();
				cout << endl;
				t2->printlist();
			
				jTable[0]->printhash();
				jTable[1]->printhash();
			
				for (int i = 0 ; i < schemaSize ; i++) {
					delete jTable[i];
				}
				delete[] jTable;				
				v->printlist();
				delete v;
				free(schema);
				break;
			case DefineSchema: 
				processDefineSchema((DefineSchema_t*)body);
				jTable = new Journal*[schemaSize];
				for (int i = 0 ; i < schemaSize ; i++) {
					jTable[i] = new Journal(schema[i]+1);
					//printf("%d\n",schema[i]);
				}
				break;
			case Transaction: 
				processTransaction(jTable,(Transaction_t*)body); 
				break;
			case ValidationQueries: 
				processValidationQueries(v,(ValidationQueries_t*)body); 
				break;
			case Flush:
				processFlush(v,jTable,(Flush_t*)body);
				break;
			case Forget: processForget((Forget_t*)body); break;
			default:
			return -1;
		}
	}
	return 0;
	
}
