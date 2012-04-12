#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>


#define BUF_SIZE 1024

typedef enum Type {
	TY_Car, TY_Cdr, TY_Op, TY_Value, TY_Str, TY_Defun, TY_If, TY_Setq, TY_EOL
} Type;

typedef struct cons_t {
	Type type;
	union {
		struct cons_t *car;
		int ivalue;
		char *svalue;
	};
	size_t len;
	struct cons_t *cdr;
} cons_t;


typedef struct list_string_t {
	char *str;
	size_t size;
	Type type;
	struct list_string_t *next;

} list_string_t;

typedef enum V_Type{ 
	Integer, Pointer, Boolean, POP_NULL, CALLFUNCTION 
} V_Type;

typedef struct value_t {
	V_Type type;
	int num;
	char *svalue; //Hash's key.
	size_t len;
} value_t;

typedef struct stack_cont_t {
	value_t *v;
	struct stack_cont_t *next;
} stack_cont_t;

typedef struct stack_t {
	int size;
	stack_cont_t *iterator;
} stack_t;


typedef enum Command {
	C_Put, C_PutObject, C_LoadValue, C_OptPlus, C_OptMinus, C_OptMul, C_OptDiv, C_OptLt, C_OptGt, C_Print, C_Call, C_TJump, C_Jump,C_Tag, C_End
} Command;

typedef struct list_run_t {
	Command command;
	value_t *v;
	struct list_run_t *next;
} list_run_t;

typedef struct st_table_t {
	int num_bins;
	int num_entries;
	struct st_table_entry_t **bins;
	struct st_table_t *next;
} st_table_t;

typedef struct st_table_entry_t {
	unsigned int hash;
	char *key;
	union {
		list_run_t *list;
		value_t *v;
	};
	struct st_table_entry_t *next;
} st_table_entry_t;

extern list_run_t *ListRun_New();

extern void freelist_string(list_string_t *p);

extern void parse(list_string_t *list, cons_t *node);

extern void vm_exec(list_run_t *root,stack_t *st,st_table_t *hash);

//eval.c
extern int eval(cons_t *p);

//cons.c
extern void dumpCons_t(cons_t * p);

extern void freeCons_t(cons_t * p);

//lex.c
extern void dumpLexer(list_string_t *p);

extern void startLex(list_string_t *p,FILE *fp);

//runtime.c
extern void compile(cons_t *ast,list_run_t *root,st_table_t *hash);

extern void vm_exec(list_run_t *root,stack_t *st,st_table_t *hash);

extern void freeListRun(list_run_t *p);

//stack.c
extern void freeStack(stack_t *self);

extern value_t *pop(stack_t *self);

extern void push(stack_t *self,value_t *v);

extern stack_t *stack_init();

//hash.c
extern st_table_t *HashTable_init();

extern list_run_t *HashTable_lookup_Function(st_table_t *self,char *key, size_t len);

extern value_t *HashTable_lookup_Value(st_table_t *self,char *key, size_t len);

extern void HashTable_insert_Function(st_table_t *self,char *key, size_t len, list_run_t *list);

extern void HashTable_insert_Value(st_table_t *self,char *key, size_t len, value_t *v);

extern void HashTable_free(st_table_t *self);

extern st_table_t *HashTable_createLocal(st_table_t *self);

extern st_table_t *HashTable_freeLocal(st_table_t *self);

