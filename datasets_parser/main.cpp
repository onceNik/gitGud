/*
 * Inspired from SIGMOD Programming Contest 2015.
 * http://db.in.tum.de/sigmod15contest/task.html
 * Simple requests parsing and reporting.
**/
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <inttypes.h>

typedef unsigned luint32_t;
typedef unsigned long luint64_t;
//---------------------------------------------------------------------------
/// Message types
typedef enum { Done, DefineSchema, Transaction, ValidationQueries, Flush, Forget } Type_t;

/// Support operations
typedef enum { Equal, NotEqual, Less, LessOrEqual, Greater, GreaterOrEqual } Op_t;

//---------------------------------------------------------------------------
typedef struct MessageHead {
   /// Total message length, excluding this head
   luint32_t messageLen;
   /// The message type
   Type_t type;
} MessageHead_t;

//---------------------------------------------------------------------------
typedef struct DefineSchema {
   /// Number of relations
   luint32_t relationCount;
   /// Column counts per relation, one count per relation. The first column is always the primary key
   luint32_t columnCounts[];
} DefineSchema_t;

//---------------------------------------------------------------------------
typedef struct Transaction {
   /// The transaction id. Monotonic increasing
   luint64_t transactionId;
   /// The operation counts
   luint32_t deleteCount,insertCount;
   /// A sequence of transaction operations. Deletes first, total deleteCount+insertCount operations
   char operations[];
} Transaction_t;

//---------------------------------------------------------------------------
typedef struct TransactionOperationDelete {
   /// The affected relation
   luint32_t relationId;
   /// The row count
   luint32_t rowCount;
   /// The deleted values, rowCount primary keyss
   luint64_t keys[];
} TransactionOperationDelete_t;

//---------------------------------------------------------------------------
typedef struct TransactionOperationInsert {
   /// The affected relation
   luint32_t relationId;
   /// The row count
   luint32_t rowCount;
   /// The inserted values, rowCount*relation[relationId].columnCount values
   luint64_t values[];
} TransactionOperationInsert_t;

//---------------------------------------------------------------------------
typedef struct ValidationQueries {
   /// The validation id. Monotonic increasing
   luint64_t validationId;
   /// The transaction range
   luint64_t from,to;
   /// The query count
   luint32_t queryCount;
   /// The queries
   char queries[];
} ValidationQueries_t;

//---------------------------------------------------------------------------
typedef struct Column {
  /// The column id
  luint32_t column;
  /// The operations
  Op_t op;
  /// The constant
  luint64_t value;
} Column_t;

//---------------------------------------------------------------------------
typedef struct Query {

   /// The relation
   luint32_t relationId;
   /// The number of bound columns
   luint32_t columnCount;
   /// The bindings
   Column_t columns[];
} Query_t;

//---------------------------------------------------------------------------
typedef struct Flush {
   /// All validations to this id (including) must be answered
   luint64_t validationId;
} Flush_t;

//---------------------------------------------------------------------------
typedef struct Forget {
   /// Transactions older than that (including) will not be tested for
   luint64_t transactionId;
} Forget_t;


static luint32_t* schema = NULL;
static void processDefineSchema(DefineSchema_t *s){
  int i;
  printf("DefineSchema %d |", s->relationCount);
  if ( schema == NULL) free(schema);
  schema = (luint32_t*)malloc(sizeof(luint32_t)*s->relationCount);
  
  for(i = 0; i < s->relationCount; i++) {
    printf(" %d ",s->columnCounts[i]);
    schema[i] = s->columnCounts[i];
  }
  printf("\n");
}

static void processTransaction(Transaction_t *t){
  int i;
  const char* reader = t->operations;
  printf("Transaction %lu (%u, %u) |", t->transactionId, t->deleteCount, t->insertCount);
  for(i=0; i < t->deleteCount; i++) {
    const TransactionOperationDelete_t* o = (TransactionOperationDelete_t*)reader;
    printf("opdel rid %u #rows %u ", o->relationId, o->rowCount);
    reader+=sizeof(TransactionOperationDelete_t)+(sizeof(luint64_t)*o->rowCount);
  }
  printf(" \t| ");
  for(i=0; i < t->insertCount; i++) {
    const TransactionOperationInsert_t* o = (TransactionOperationInsert_t*)reader;
    printf("opins rid %u #rows %u |", o->relationId, o->rowCount);
    reader+=sizeof(TransactionOperationInsert_t)+(sizeof(luint64_t)*o->rowCount*schema[o->relationId]);
  }
  printf("\n");
  
}
static void processValidationQueries(ValidationQueries_t *v){
  printf("ValidationQueries %lu [%lu, %lu] %u\n", v->validationId, v->from, v->to, v->queryCount);
}
static void processFlush(Flush_t *fl){
 printf("Flush %lu\n", fl->validationId);

}
static void processForget(Forget_t *fo){
  printf("Forget %lu\n", fo->transactionId);

}


int main(int argc, char **argv) {

  MessageHead_t head;
  void *body = NULL;
  luint32_t len;

    while(1){
      // Retrieve the message head
      if (read(0, &head, sizeof(head)) <= 0) { return -1; } // crude error handling, should never happen
      printf("HEAD LEN %u \t| HEAD TYPE %u \t| DESC ", head.messageLen, head.type);
        
      // Retrieve the message body
      if (body != NULL) free(body);
      if (head.messageLen > 0 ){
      body = malloc(head.messageLen*sizeof(char));
      if (read(0, body, head.messageLen) <= 0) { printf("err");return -1; } // crude error handling, should never happen
      len-=(sizeof(head) + head.messageLen);
      }
            
      // And interpret it
      switch (head.type) {
         case Done: printf("\n");return 0;
         case DefineSchema: processDefineSchema((DefineSchema_t*)body); break;
         case Transaction: processTransaction((Transaction_t*)body); break;
         case ValidationQueries: processValidationQueries((ValidationQueries_t*)body); break;
         case Flush: processFlush((Flush_t*)body); break;
         case Forget: processForget((Forget_t*)body); break;
         default: 
         return -1; // crude error handling, should never happen
      }
    }
    
  return 0;
}
