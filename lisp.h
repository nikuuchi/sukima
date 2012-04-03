#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define BUF_SIZE 1024
#define BINS 64

typedef enum Type {TY_Car,TY_Cdr,TY_Op,TY_Value,TY_Str,TY_Defun,TY_If,TY_Setq} Type;

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


extern void HashTable_init(st_table *table);

extern unsigned int getHashNumber(char * s);

extern void freeCons_t(cons_t * p);

extern list_string *lex(list_string *list,char * buf,int size);

extern void freelist_string(list_string *p);

extern void parse(list_string *list, cons_t *node);
