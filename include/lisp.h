#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <readline/readline.h>
#include <readline/history.h>

extern const void **tables;

#define BUF_SIZE 4096

typedef enum Type {
	TY_LParen, TY_RParen, TY_Op,
	TY_List,   TY_Int,    TY_Double, TY_Boolean,
	TY_CStr,   TY_Str,    TY_Defun,
	TY_If,     TY_Setq,   TY_EOL
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

typedef union value_t {
	void *o;
	struct string *string;
	uint64_t bytes;
	double d;
	int i;
} value_t;

typedef enum OpCode {
	C_Put,    C_SetHash, C_LoadValue, C_Inc, C_Dec,
	C_OpPlus, C_OpMinus, C_OpMul,
	C_OpDiv,  C_OpMod,   C_OpLt,
	C_OpGt,   C_OpEq, C_OpEqLt, C_OpEqGt ,C_OpCPlus,
	C_OpCMinus, C_OpCMul, 
	C_OpCDiv,  C_OpCMod,   C_OpCLt,
	C_OpCGt,   C_OpCEq, C_OpCEqLt, C_OpCEqGt, C_Print,
	C_Call, C_ExCall ,   C_TJump,   C_Nop, 
	C_Args ,C_Ret, C_UnOpPlus, C_UnOpMinus, C_UnOpMul, C_UnOpDiv, C_UnOpMod, C_UnOpT
} OpCode;

typedef struct bytecode_t {
	OpCode code;
	const void *iseq;
	value_t data;
	struct bytecode_t *next;
} bytecode_t;

#define BINS 16
typedef enum entryType {
	entryFunction, entryValue
} entryType;

typedef struct hash_entry_t {
	unsigned int hash;
	char *key;
	entryType type;
	value_t v;
	struct hash_entry_t *next;
} hash_entry_t;

typedef struct hash_table_t {
	int num_bins;
	int num_entries;
	struct hash_entry_t **bins;
	struct hash_table_t *next;
} hash_table_t;

typedef struct List_t {
	value_t *container;
	struct List_t *next;
} List_t;

#define Bytecode_New() (bytecode_t *)calloc(1,sizeof(bytecode_t));

extern void token_free(token_t *p);

extern void parse(token_t *list, cons_t *node);

extern const void **vm_exec(bytecode_t *root,int esp,hash_table_t *hash, int table_flag);

//excall.c
extern int excall(char *name,size_t name_len,value_t *st,int esp);

//cons.c
#define Cons_New() (cons_t *)calloc(1,sizeof(cons_t))

extern void dumpCons_t(cons_t * p);

extern void freeCons_t(cons_t * p);

//lex.c
extern void dumpLexer(token_t *p);

extern void startLex(token_t *p,FILE *fp);

extern token_t *lex(token_t *list,char * buf,int size);

//codegen.c
extern void compile(cons_t *ast,bytecode_t *root,hash_table_t *hash);

extern void Bytecode_free(bytecode_t *p);

//hash.c
extern hash_table_t *HashTable_init();

extern bytecode_t *HashTable_lookup_Function(hash_table_t *self,char *key, size_t len);

extern value_t HashTable_lookup_Value(hash_table_t *self,char *key, size_t len);

extern void HashTable_insert_Function(hash_table_t *self,char *key, size_t len, bytecode_t *list);

extern void HashTable_insert_Value(hash_table_t *self,char *key, size_t len, value_t v);

extern void HashTable_free(hash_table_t *self);

extern hash_table_t *HashTable_createLocal(hash_table_t *self);

extern hash_table_t *HashTable_freeLocal(hash_table_t *self);

//boxing

#define NaN       (uint64_t)(0xFFF0000000000000)
#define Mask      (uint64_t)(0x00000000FFFFFFFF)
#define TYPE      (uint64_t)(0x000F000000000000) // for typecheck

#define IntTag    (uint64_t)(0x0001000000000000) //Type int
#define True      (uint64_t)(0xFFF2000000000001) //Type bool
#define False     (uint64_t)(0xFFF2000000000000)

#define ListTag   (uint64_t)(0x0003000000000000) //Type List
#define StringTag (uint64_t)(0x0004000000000000) //Type String

#define Int_init(b) (value_t)(((b) & Mask) | NaN | IntTag)
#define Double_init(b) (value_t)((b))

#define Boolean_init(b) (value_t)(False | (b))

static inline int Type_Check(value_t t){
	return (((t.bytes & NaN) == NaN) * ((t.bytes & TYPE) >> 48 ));
}

#define String_Ptr(a) ((struct string *)((a).bytes ^ (NaN |StringTag)))

#define String_Copy(p,buf,size) \
do { \
	(p) = (char *)malloc((size) + 1); \
	(p) = strncpy((p),(buf),(size)); \
	(p)[(size)] = '\0'; \
}while(0);

//stack operation
#define push(a) st[esp++] = (a)
#define pop() (st[--esp])


//for excall

#define METHOD(module,name,argc) {#name, sizeof(#name), (argc),SKM_##module##_##name }

typedef value_t dynamic;

typedef struct skm_method_data {
	char *name;
	size_t len;
	int argc;
	dynamic(* ex_method)(value_t *args);

} skm_method_data;

