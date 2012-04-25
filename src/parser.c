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
		printf("parse error. %s \n",at(list,n)->str); \
		exit(1); \
	} while(0);

int F_LParen(token_t *list, cons_t *node,int n);
int F_Op(token_t *list, cons_t *node, int n);
int F_Int(token_t *list, cons_t *node,int n);
int F_Double(token_t *list, cons_t *node,int n);

token_t *at(token_t *list,int n)
{
	token_t *p = list;
	int i=0;
	for(i=0;i<n;++i)p = p->next;
	return p;
}

void parse(token_t *list, cons_t *node)
{
//	printf("car:%d:cdr:%d:op:%d:value:%d:str:%d:setq:%d\n",TY_LParen,TY_RParen,TY_Op,TY_Int,TY_Str,TY_Setq);
	int n = 0;
//	printf("type:%d:n:%d\n",at(list,n)->type,n);
	switch(at(list,n)->type) {
	case TY_LParen:
		n = F_LParen(list,node,n);
		break;
	case TY_Int:
		n = F_Int(list,node,n);
		break;
	case TY_Double:
		n = F_Double(list,node,n);
		break;
	default:
		DEFAULT(node);
	}
	
}

int F_LParen(token_t *list, cons_t *node,int n)
{
	node->type = TY_LParen;
	node->car = Cons_New();
	++n;
//	printf("car1:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type) {
	case TY_LParen:
		n = F_LParen(list,node->car,n);
		break;
	case TY_RParen:
		CASE_RParen(node); //Need check
		break;
	case TY_Op:
	case TY_Str:
	case TY_Setq:
	case TY_Defun:
	case TY_If:
		n = F_Op(list,node->car,n);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node->car);
	}

//	printf("car2:type:%d:n:%d\n",at(list,n)->type,n);

	node->cdr = Cons_New();

	switch(at(list,n)->type) {
	case TY_LParen:
		n = F_LParen(list,node->cdr,n);
		break;
	case TY_RParen:
		CASE_RParen(node->cdr);
		break;
	case TY_Op:
	case TY_Str:
		n = F_Op(list,node->cdr,n);
		break;
	case TY_Int:
		n = F_Int(list,node->cdr,n);
		break;
	case TY_Double:
		n = F_Double(list,node->cdr,n);
		break;
	case TY_EOL:
		CASE_END(node->cdr);
		break;
	default:
		DEFAULT(node->cdr);
	}
	return n;
}

int F_Op(token_t *list, cons_t *node,int n)
{
	token_t *c = at(list,n);
	node->type = c->type;
	String_Copy(node->string.s,c->str,c->size);
	node->string.len = c->size;
	node->cdr = Cons_New();
	node = node->cdr;
	++n;
//	printf("op:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type) {
	case TY_LParen:
		n = F_LParen(list,node,n);
		break;
	case TY_RParen:
		CASE_RParen(node);
		break;
	case TY_Int:
		n = F_Int(list,node,n);
		break;
	case TY_Double:
		n = F_Double(list,node,n);
		break;
	case TY_Str:
		n = F_Op(list,node,n);
		break;
	case TY_EOL:
		CASE_END(node);
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

int F_Int(token_t *list, cons_t *node,int n)
{
	node->type = TY_Int;
	node->ivalue = atoi(at(list,n)->str);
	node->cdr = Cons_New();
	++n;
	node = node->cdr;
//	printf("value:type:%d:n:%d\n",at(list,n)->type,n);
	switch(at(list,n)->type) {
	case TY_LParen:
		n = F_LParen(list,node,n);
		break;
	case TY_RParen:
		CASE_RParen(node);
		break;
	case TY_Int:
		n = F_Int(list,node,n);
		break;
	case TY_Double:
		n = F_Double(list,node,n);
		break;
	case TY_Str:
		n = F_Op(list,node,n);
		break;
	case TY_EOL:
		CASE_END(node);
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

int F_Double(token_t *list, cons_t *node,int n)
{
	node->type = TY_Double;
	node->fvalue = atof(at(list,n)->str);
	node->cdr = Cons_New();
	++n;
	node = node->cdr;
	//printf("value:type:%d:n:%d\n",at(list,n)->type,n);
	switch(at(list,n)->type) {
	case TY_LParen:
		n = F_LParen(list,node,n);
		break;
	case TY_RParen:
		CASE_RParen(node);
		break;
	case TY_Int:
		n = F_Int(list,node,n);
		break;
	case TY_Double:
		n = F_Double(list,node,n);
		break;
	case TY_Str:
		n = F_Op(list,node,n);
		break;
	case TY_EOL:
		CASE_END(node);
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

