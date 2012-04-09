#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


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
} value_t;

typedef struct stack_cont_t {
	value_t v;
	struct stack_cont_t *next;
} stack_cont_t;

typedef struct stack_t {
	int size;
	stack_cont_t *iterator;	
} stack_t;


typedef enum Command {
	C_Put, C_OptPlus, C_OptMinus, C_OptMul, C_OptDiv, C_OptLt, C_OptGt, C_Print, C_Call, C_End
} Command;

typedef struct list_run_t {
	Command command;
	value_t v;
	struct list_run_t *next;
} list_run_t;

typedef struct st_table_t {
	int num_bins;
	int num_entries;
	unsigned int (*getHash)(struct st_table_t *,char *);
	struct st_table_entry_t **bins;
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


extern st_table_t *HashTable_init();

extern void freelist_string(list_string_t *p);

extern void parse(list_string_t *list, cons_t *node);

//eval.c
extern int eval(cons_t *p);

//cons.c
extern void dumpCons_t(cons_t * p);

extern void freeCons_t(cons_t * p);

//lex.c
extern void dumpLexer(list_string_t *p);

extern void startLex(list_string_t *p,FILE *fp);

//runtime.c
extern void run(cons_t *ast);

//stack.c
extern void freeStack(stack_t *self);

extern value_t pop(stack_t *self);

extern void push(stack_t *self,value_t v);

extern stack_t *stack_init();

