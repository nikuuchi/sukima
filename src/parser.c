#include "lisp.h"

#define CASE_CDR(c) \
	do { \
		(c)->type = TY_Cdr; \
		(c)->svalue = ")"; \
		(c)->len = 1; \
		++n; \
	} while(0);

#define DEFAULT(c) \
	do { \
		printf("parse error. %s \n",at(list,n)->str); \
		exit(1); \
	} while(0);
#define Cons_New() (cons_t *)malloc(sizeof(cons_t))

int F_Car(list_string_t *list, cons_t *node,int n);
int F_Op(list_string_t *list, cons_t *node, int n);
int F_Value(list_string_t *list, cons_t *node,int n);

list_string_t *at(list_string_t *list,int n)
{
	list_string_t *p = list;
	int i=0;
	for(i=0;i<n;++i)p = p->next;
	return p;
}

void parse(list_string_t *list, cons_t *node)
{
//	printf("car:%d:cdr:%d:op:%d:value:%d:str:%d:setq:%d\n",TY_Car,TY_Cdr,TY_Op,TY_Value,TY_Str,TY_Setq);
	int n = 0;
//	printf("type:%d:n:%d\n",at(list,n)->type,n);
	Type t = TY_Car;
	while((t = at(list,n)->type) != TY_EOL) {
		switch(t) {
		case TY_Car:
			n = F_Car(list,node,n);
			break;
		default:
			DEFAULT(node);
		}
	}
	
}

int F_Car(list_string_t *list, cons_t *node,int n)
{
	node->type = TY_Car;
	node->car = Cons_New();
	++n;
//	printf("car1:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type) {
	case TY_Car:
		n = F_Car(list,node->car,n);
		break;
	case TY_Cdr:
		CASE_CDR(node); //Need check
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
	case TY_Car:
		n = F_Car(list,node->cdr,n);
		break;
	case TY_Cdr:
		CASE_CDR(node->cdr);
		break;
	case TY_Op:
	case TY_Str:
		n = F_Op(list,node->cdr,n);
		break;
	case TY_Value:
		n = F_Value(list,node->cdr,n);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node->cdr);
	}
	return n;
}

int F_Op(list_string_t *list, cons_t *node,int n)
{
	list_string_t *c = at(list,n);
	node->type = c->type;
	node->svalue = c->str;
	node->len = c->size;
	node->cdr = Cons_New();
	node = node->cdr;
	++n;
//	printf("op:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type) {
	case TY_Car:
		n = F_Car(list,node,n);
		break;
	case TY_Cdr:
		CASE_CDR(node);
		break;
	case TY_Value:
		n = F_Value(list,node,n);
		break;
	case TY_Str:
		n = F_Op(list,node,n);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

int F_Value(list_string_t *list, cons_t *node,int n)
{
	node->type = TY_Value;
	node->ivalue = atoi(at(list,n)->str);
	node->cdr = Cons_New();
	++n;
	node = node->cdr;
//	printf("value:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type) {
	case TY_Car:
		n = F_Car(list,node,n);
		break;
	case TY_Cdr:
		CASE_CDR(node);
		break;
	case TY_Value:
		n = F_Value(list,node,n);
		break;
	case TY_Str:
		n = F_Op(list,node,n);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

