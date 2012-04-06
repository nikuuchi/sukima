#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define BUF_SIZE 1024
#define BINS 64

typedef enum Type {TY_Car,TY_Cdr,TY_Op,TY_Value,TY_Str,TY_Defun,TY_If,TY_Setq,TY_EOL} Type;

typedef struct cons_t
{
	Type type;
	union {
		struct cons_t *car;
		int ivalue;
		char *svalue;
	};
	struct cons_t *cdr;
} cons_t;

typedef struct st_table
{
	int num_bins;
	int num_entries;
	struct st_table_entry **bins;
} st_table;

typedef struct st_table_entry
{
	unsigned int hash;
	char *key;
	cons_t *value;
	struct st_table_entry *next;
} st_table_entry;

typedef struct list_string
{
	char *str;
	size_t size;
	Type type;
	struct list_string *next;

} list_string;



typedef struct stack{
	cons_t * cont;
	struct stack *next;
} stack;


extern void HashTable_init(st_table *table);

extern unsigned int getHashNumber(char * s);

extern void freeCons_t(cons_t * p);

extern void freelist_string(list_string *p);

extern void parse(list_string *list, cons_t *node);

extern cons_t *pop(stack *self);

extern void push(stack *self,cons_t * p);

extern int eval(cons_t *p);

extern void dumpCons_t(cons_t * p);

extern void dumpLexer(list_string *p);

extern void startLex(list_string *p,FILE *fp);
