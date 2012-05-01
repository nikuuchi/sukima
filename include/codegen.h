#include "lisp.h"


#define Bytecode_next(list) \
	do { \
		(list)->next = Bytecode_New(); \
		(list) = (list)->next; \
	} while(0);

static bytecode_t *asm_LParen           (bytecode_t *opcode,cons_t *cons,hash_table_t *arguments,hash_table_t *hash);
static bytecode_t *asm_Setq             (bytecode_t *opcode,cons_t *cons,hash_table_t *arguments,hash_table_t *hash);
static bytecode_t *asm_If               (bytecode_t *opcode,cons_t *cons,hash_table_t *arguments,hash_table_t *hash);
static bytecode_t *asm_Defun            (bytecode_t *opcode,cons_t *cons,hash_table_t *arguments,hash_table_t *hash);
static bytecode_t *asm_Op               (bytecode_t *opcode,cons_t *cons,hash_table_t *arguments,hash_table_t *hash);
static bytecode_t *asm_CallFunction     (bytecode_t *opcode,cons_t *cons,hash_table_t *arguments,hash_table_t *hash);
static bytecode_t *assemble             (bytecode_t *opcode,cons_t *cons,hash_table_t *arguments,hash_table_t *hash);

