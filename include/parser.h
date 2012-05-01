#include "lisp.h"

typedef token_t array_token_t;

static int F_LParen(array_token_t *array, cons_t *node,int n);
static int F_Op(array_token_t *array, cons_t *node, int n);
static int F_Int(array_token_t *array, cons_t *node,int n);
static int F_Double(array_token_t *array, cons_t *node,int n);
static int F_Boolean(array_token_t *array, cons_t *node,int n);
static int F_CStr(array_token_t *array, cons_t *node,int n);
