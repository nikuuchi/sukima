#include "lisp.h"

typedef token_t array_token_t;

int F_LParen(array_token_t *array, cons_t *node,int n);
int F_Op(array_token_t *array, cons_t *node, int n);
int F_Int(array_token_t *array, cons_t *node,int n);
int F_Double(array_token_t *array, cons_t *node,int n);
int F_Boolean(array_token_t *array, cons_t *node,int n);
int F_CStr(array_token_t *array, cons_t *node,int n);

static int CASE_RParen(cons_t *c,int n)
{
	c->type = TY_RParen;
	c->string.s = ")"; 
	c->string.len = 1; 
	return ++n;
}

static int CASE_END(cons_t *c,int n)
{
	c->type = TY_EOL;
	c->string.s = "EOL";
	c->string.len = 3;
	return ++n;
}

static void DEFAULT(array_token_t *array,int n)
{
	printf("parse error. %s \n",array[n].str);
	exit(1);
}

static int token_size(token_t *list)
{
	int i = 0;
	while(list != NULL) {
		list = list->next;
		++i;
	}
	return i;
}

static void list2array(array_token_t *array,token_t *list,int size)
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
	case TY_Boolean:
		n = F_Boolean(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	default:
		DEFAULT(array,n);
	}
	free(array);
}

int F_LParen(array_token_t *array, cons_t *node,int n)
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
		n = CASE_RParen(node->car,n); //Need check
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
		DEFAULT(array,n);
	}

//	printf("car2:type:%d:n:%d\n",at(array,n)->type,n);

	node->cdr = Cons_New();

	switch(array[n].type) {
	case TY_LParen:
		n = F_LParen(array,node->cdr,n);
		break;
	case TY_RParen:
		n = CASE_RParen(node->cdr,n);
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
	case TY_Boolean:
		n = F_Boolean(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node->cdr,n);
		break;
	case TY_EOL:
		n = CASE_END(node->cdr,n);
		break;
	default:
		DEFAULT(array,n);
	}
	return n;
}

int F_Op(array_token_t *array, cons_t *node,int n)
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
		n = CASE_RParen(node,n);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_Boolean:
		n = F_Boolean(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	case TY_Str:
		n = F_Op(array,node,n);
		break;
	case TY_EOL:
		n = CASE_END(node,n);
		break;
	default:
		DEFAULT(array,n);
	}
	return n;
}

int F_Int(array_token_t *array, cons_t *node,int n)
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
		n = CASE_RParen(node,n);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_Boolean:
		n = F_Boolean(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	case TY_Str:
		n = F_Op(array,node,n);
		break;
	case TY_EOL:
		n = CASE_END(node,n);
		break;
	default:
		DEFAULT(array,n);
	}
	return n;
}

int F_Double(array_token_t *array, cons_t *node,int n)
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
		n = CASE_RParen(node,n);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_Boolean:
		n = F_Boolean(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	case TY_Str:
		n = F_Op(array,node,n);
		break;
	case TY_EOL:
		n = CASE_END(node,n);
		break;
	default:
		DEFAULT(array,n);
	}
	return n;
}

int F_Boolean(array_token_t *array, cons_t *node,int n)
{
	node->type = TY_Boolean;
	if(strcmp("T",array[n].str) == 0) {
		node->ivalue = 1;
	}else {
		node->ivalue = 0;
	}
	node->cdr = Cons_New();
	++n;
	node = node->cdr;

	//printf("value:type:%d:n:%d\n",at(array,n)->type,n);
	switch(array[n].type) {
	case TY_LParen:
		n = F_LParen(array,node,n);
		break;
	case TY_RParen:
		n = CASE_RParen(node,n);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_Boolean:
		n = F_Boolean(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	case TY_Str:
		n = F_Op(array,node,n);
		break;
	case TY_EOL:
		n = CASE_END(node,n);
		break;
	default:
		DEFAULT(array,n);
	}
	return n;
}

int F_CStr(array_token_t *array, cons_t *node,int n)
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
		n = CASE_RParen(node,n);
		break;
	case TY_Int:
		n = F_Int(array,node,n);
		break;
	case TY_Double:
		n = F_Double(array,node,n);
		break;
	case TY_Boolean:
		n = F_Boolean(array,node,n);
		break;
	case TY_CStr:
		n = F_CStr(array,node,n);
		break;
	case TY_Str:
		n = F_Op(array,node,n);
		break;
	case TY_EOL:
		n = CASE_END(node,n);
		break;
	default:
		DEFAULT(array,n);
	}
	return n;
}

