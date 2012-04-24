#include "nanbox.h"

#include <readline/readline.h>
#include <readline/history.h>

extern void **tables;

#define BUF_SIZE 1024

typedef enum Type {
	TY_Car, TY_Cdr, TY_Op, TY_Int, TY_Float, TY_Str, TY_Defun, TY_If, TY_Setq, TY_EOL
} Type;

typedef struct cons_t {
	Type type;
	union {
		struct cons_t *car;
		int ivalue;
		double fvalue;
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

struct command_t;
/*
typedef struct stack_frame_t {
	int returnAddr;
	int argc;
	int locals;
	value_t **fields; //type will change

} stack_frame_t;
*/

typedef union value_t {
	void *pointer;
	struct string *string;
	uint64_t bytes;
	double d;
	int i;
} value_t;

typedef enum Command {
	C_Put, C_SetHash, C_LoadValue, C_OpPlus, C_OpMinus, C_OpMul, C_OpDiv, C_OpLt, C_OpGt, C_Print, C_Call, C_TJump,C_Tag, C_Args ,C_End
} Command;

typedef struct command_t {
	Command command;
	value_t data[2];
	const void *iseq;
	struct command_t *next;
} command_t;

typedef struct hash_table_t {
	int num_bins;
	int num_entries;
	struct hash_entry_t **bins;
	struct hash_table_t *next;
} hash_table_t;

typedef enum entryType {
	entryFunction, entryValue
} entryType;

typedef struct hash_entry_t {
	unsigned int hash;
	char *key;
	entryType type;
	union {
		command_t *list;
		value_t *v;
	};
	struct hash_entry_t *next;
} hash_entry_t;

extern command_t *ListRun_New();

extern void freelist_string(token_t *p);

extern void parse(token_t *list, cons_t *node);

extern void **vm_exec(command_t *root,value_t st[],int esp,hash_table_t *hash, int table_flag);

//eval.c
extern int eval(cons_t *p);

//cons.c
extern void dumpCons_t(cons_t * p);

extern void freeCons_t(cons_t * p);

//lex.c
extern void dumpLexer(token_t *p);

extern void startLex(token_t *p,FILE *fp);

extern token_t *lex(token_t *list,char * buf,int size);

//codegen.c
extern void compile(cons_t *ast,command_t *root,hash_table_t *hash);

extern void freeListRun(command_t *p);

//hash.c
extern hash_table_t *HashTable_init();

extern command_t *HashTable_lookup_Function(hash_table_t *self,char *key, size_t len);

extern value_t *HashTable_lookup_Value(hash_table_t *self,char *key, size_t len);

extern void HashTable_insert_Function(hash_table_t *self,char *key, size_t len, command_t *list);

extern void HashTable_insert_Value(hash_table_t *self,char *key, size_t len, value_t *v);

extern void HashTable_free(hash_table_t *self);

extern hash_table_t *HashTable_createLocal(hash_table_t *self);

extern hash_table_t *HashTable_freeLocal(hash_table_t *self);

