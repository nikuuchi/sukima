#include "lisp.h"

typedef token_t array_token_t;

static int F_LParen(array_token_t *array, cons_t *node,int n);
static int F_Method(array_token_t *array, cons_t *node, int n);
static int F_Operand(array_token_t *array, cons_t *node,int n);
