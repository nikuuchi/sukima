#include "lisp.h"

#define CASE_RParen(c) \
	do { \
		(c)->type = TY_RParen; \
		(c)->string.s = ")"; \
		(c)->string.len = 1; \
		++n; \
	} while(0);

#define CASE_END(c) \
	do { \
		(c)->type = TY_EOL; \
		(c)->string.s = "EOL"; \
		(c)->string.len = 3; \
		++n; \
	} while(0);

#define DEFAULT(c) \
	do { \
		printf("parse error. %s \n",array[n].str); \
		exit(1); \
	} while(0);

int F_LParen(token_t array[], cons_t *node,int n);
int F_Op(token_t array[], cons_t *node, int n);
int F_Int(token_t array[], cons_t *node,int n);
int F_Double(token_t array[], cons_t *node,int n);
int F_CStr(token_t array[], cons_t *node,int n);

static int token_size(token_t *list)
{
	int i = 0;
	while(list != NULL) {
		list = list->next;
		++i;
	}
	return i;
}

static void list2array(token_t array[],token_t *list,int size)
{
	int i = 0;
	token_t *p = list;
	for(;i<size;++i) {
		array[i] = *p;
		p = p->next;
	}
}

void parse(token_t *list, cons_t *node)
{
//	printf("car:%d:cdr:%d:op:%d:value:%d:str:%d:setq:%d\n",TY_LParen,TY_RParen,TY_Op,TY_Int,TY_Str,TY_Setq);
	int size = token_size(list);
	token_t *array = (token_t *)malloc(size * sizeof(token_t));
	list2array(array,list,size);

	int n = 0;
	switch(array[n].type) {
	case TY_LParen:
		n = F_LParen(array,node,n);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	default:
		DEFAULT(node);
	}
	free(array);
}

int F_LParen(token_t array[], cons_t *node,int n)
{
	node->type = TY_LParen;
	node->car = Cons_New();
	++n;
//	printf("car1:type:%d:n:%d\n",at(array,n)->type,n);

	switch(array[n].type) {
	case TY_LParen:
		n = F_LParen(array,node->car,n);
		break;
	case TY_RParen:
		CASE_RParen(node); //Need check
		break;
	case TY_Op:
	case TY_Str:
	case TY_Setq:
	case TY_Defun:
	case TY_If:
		n = F_Op(array,node->car,n);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node->car);
	}

//	printf("car2:type:%d:n:%d\n",at(array,n)->type,n);

	node->cdr = Cons_New();

	switch(array[n].type) {
	case TY_LParen:
		n = F_LParen(array,node->cdr,n);
		break;
	case TY_RParen:
		CASE_RParen(node->cdr);
		break;
	case TY_Op:
	case TY_Str:
		n = F_Op(array,node->cdr,n);
		break;
	case TY_Int:
		n = F_Int(array,node->cdr,n);
		break;
	case TY_Double:
		n = F_Double(array,node->cdr,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node->cdr,n);
		break;
	case TY_EOL:
		CASE_END(node->cdr);
		break;
	default:
		DEFAULT(node->cdr);
	}
	return n;
}

int F_Op(token_t array[], cons_t *node,int n)
{
	node->type = array[n].type;
	String_Copy(node->string.s,array[n].str,array[n].size);
	node->string.len = array[n].size;
	node->cdr = Cons_New();
	node = node->cdr;
	++n;
//	printf("op:type:%d:n:%d\n",at(list,n)->type,n);

	switch(array[n].type) {
	case TY_LParen:
		n = F_LParen(array,node,n);
		break;
	case TY_RParen:
		CASE_RParen(node);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	case TY_Str:
		n = F_Op(array,node,n);
		break;
	case TY_EOL:
		CASE_END(node);
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

int F_Int(token_t array[], cons_t *node,int n)
{
	node->type = TY_Int;
	node->ivalue = atoi(array[n].str);
	node->cdr = Cons_New();
	++n;
	node = node->cdr;
//	printf("value:type:%d:n:%d\n",at(list,n)->type,n);
	switch(array[n].type) {
	case TY_LParen:
		n = F_LParen(array,node,n);
		break;
	case TY_RParen:
		CASE_RParen(node);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	case TY_Str:
		n = F_Op(array,node,n);
		break;
	case TY_EOL:
		CASE_END(node);
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

int F_Double(token_t array[], cons_t *node,int n)
{
	node->type = TY_Double;
	node->fvalue = atof(array[n].str);
	node->cdr = Cons_New();
	++n;
	node = node->cdr;
	//printf("value:type:%d:n:%d\n",at(array,n)->type,n);
	switch(array[n].type) {
	case TY_LParen:
		n = F_LParen(array,node,n);
		break;
	case TY_RParen:
		CASE_RParen(node);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	case TY_Str:
		n = F_Op(array,node,n);
		break;
	case TY_EOL:
		CASE_END(node);
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

int F_CStr(token_t array[], cons_t *node,int n)
{
	node->type = TY_CStr;
	String_Copy(node->string.s,array[n].str,array[n].size);
	node->string.len = array[n].size;
	node->cdr = Cons_New();
	++n;
	node = node->cdr;
	//printf("value:type:%d:n:%d\n",at(array,n)->type,n);
	switch(array[n].type) {
	case TY_LParen:
		n = F_LParen(array,node,n);
		break;
	case TY_RParen:
		CASE_RParen(node);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	case TY_Str:
		n = F_Op(array,node,n);
		break;
	case TY_EOL:
		CASE_END(node);
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

