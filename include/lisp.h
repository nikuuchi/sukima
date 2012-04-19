#include "nanbox.h"

#include <readline/readline.h>
#include <readline/history.h>

extern void **tables;

#define BUF_SIZE 1024

typedef enum Type {
	TY_Car, TY_Cdr, TY_Op, TY_Value, TY_Str, TY_Defun, TY_If, TY_Setq, TY_EOL
} Type;

typedef struct cons_t {
	Type type;
	union {
		struct cons_t *car;
		int ivalue;
		struct string {
			char *s;
			size_t len;
		}string;
	};
	struct cons_t *cdr;
} cons_t;

//deprecated. It will merge cons_t soon.
typedef struct token_t {
	char *str;
	size_t size;
	Type type;
	struct token_t *next;

} token_t;

typedef enum V_Type { 
	Integer, Pointer, Boolean, POP_NULL, CALLFUNCTION, Int_push 
} V_Type;

struct list_run_t;
//deprecated. It will change NaN Boxing struct.
typedef struct value_t {
	V_Type type;
	int num;
	struct string string;
	struct list_run_t *func;
} value_t;

typedef struct stack_frame_t {
	int returnAddr;
	int argc;
	int locals;
	value_t **fields; //type will change

} stack_frame_t;

typedef union boxed_value {
	struct { uint64_t tag:4, value:60;} taged_value;
	int32_t ivalue;
	float   fvalue;
	void   *ptr;
} boxed_value;

typedef enum Command {
	C_Put, C_PutObject, C_LoadValue, C_OptPlus, C_OptMinus, C_OptMul, C_OptDiv, C_OptLt, C_OptGt, C_Print, C_Call, C_TJump, C_Jump,C_Tag, C_Args ,C_End
} Command;

typedef struct list_run_t {
	Command command;
	value_t *v;
	const void *iseq;
	struct list_run_t *next;
} list_run_t;

typedef struct st_table_t {
	int num_bins;
	int num_entries;
	struct st_table_entry_t **bins;
	struct st_table_t *next;
} st_table_t;

typedef enum entryType {
	entryFunction, entryValue
} entryType;

typedef struct st_table_entry_t {
	unsigned int hash;
	char *key;
	entryType type;
	union {
		list_run_t *list;
		value_t *v;
	};
	struct st_table_entry_t *next;
} st_table_entry_t;

extern list_run_t *ListRun_New();

extern void freelist_string(token_t *p);

extern void parse(token_t *list, cons_t *node);

extern void **vm_exec(list_run_t *root,value_t **st,int esp,st_table_t *hash, int table_flag);

//eval.c
extern int eval(cons_t *p);

//cons.c
extern void dumpCons_t(cons_t * p);

extern void freeCons_t(cons_t * p);

//lex.c
extern void dumpLexer(token_t *p);

extern void startLex(token_t *p,FILE *fp);

extern token_t *lex(token_t *list,char * buf,int size);

//runtime.c
extern void compile(cons_t *ast,list_run_t *root,st_table_t *hash);

extern void freeListRun(list_run_t *p);


//hash.c
extern st_table_t *HashTable_init();

extern list_run_t *HashTable_lookup_Function(st_table_t *self,char *key, size_t len);

extern value_t *HashTable_lookup_Value(st_table_t *self,char *key, size_t len);

extern void HashTable_insert_Function(st_table_t *self,char *key, size_t len, list_run_t *list);

extern void HashTable_insert_Value(st_table_t *self,char *key, size_t len, value_t *v);

extern void HashTable_free(st_table_t *self);

extern st_table_t *HashTable_createLocal(st_table_t *self);

extern st_table_t *HashTable_freeLocal(st_table_t *self);

