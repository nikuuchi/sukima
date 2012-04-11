#include "lisp.h"

#define CASE_CAR(c) \
	do { \
		n = F_Car(list,(c),n); \
	} while(0);

#define CASE_CDR(c) \
	do { \
		(c)->type = TY_Cdr; \
		(c)->svalue = ")"; \
		(c)->len = 1; \
		++n; \
	} while(0);

#define CASE_OP(c) \
	do { \
		n = F_Op(list,(c),n); \
	} while(0);

#define CASE_STR(c) \
	do { \
		n = F_Str(list,(c),n); \
	} while(0);

#define CASE_VALUE(c) \
	do { \
		n = F_Value(list,(c),n); \
	} while(0);

#define CASE_IF(c) \
	do { \
		n = F_If(list,(c),n); \
	} while(0);

#define CASE_DEFUN(c) \
	do { \
		n = F_Defun(list,(c),n); \
	} while(0);

#define CASE_SETQ(c) \
	do { \
		n = F_Setq(list,(c),n); \
	} while(0);

#define DEFAULT(c) \
	do { \
		F_ERROR(list,(c),n); \
		++n; \
	} while(0);
#define Cons_New() (cons_t *)malloc(sizeof(cons_t))

void F_ERROR(list_string_t *list, cons_t *node,int n);
int F_Car(list_string_t *list, cons_t *node,int n);
int F_Op(list_string_t *list, cons_t *node, int n);
int F_Cdr(list_string_t *list, cons_t *node,int n);
int F_Value(list_string_t *list, cons_t *node,int n);
int F_Str(list_string_t *list, cons_t *node,int n);
int F_Setq(list_string_t *list, cons_t *node,int n);
int F_Defun(list_string_t *list, cons_t *node,int n);
int F_If(list_string_t *list, cons_t *node,int n);

list_string_t *at(list_string_t *list,int n)
{
	list_string_t *p = list;
	int i=0;
	for(i=0;i<n;++i)p = p->next;
	return p;
}

void parse(list_string_t *list, cons_t *node)
{
	printf("car:%d:cdr:%d:op:%d:value:%d:str:%d:setq:%d\n",TY_Car,TY_Cdr,TY_Op,TY_Value,TY_Str,TY_Setq);
	int n = 0;
	printf("type:%d:n:%d\n",at(list,n)->type,n);
	while(at(list,n)->type != TY_EOL){
		
		switch(at(list,n)->type) {
		case TY_Car:
			CASE_CAR(node);
			break;
		default:
			DEFAULT(node);
		}
	}
	
}

void F_ERROR(list_string_t *list, cons_t *node,int n)
{
	printf("parse error. %s \n",at(list,n)->str);
}

int F_Car(list_string_t *list, cons_t *node,int n)
{
	node->type = TY_Car;
	node->car = Cons_New();
	++n;
	printf("car1:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type) {
	case TY_Car:
		CASE_CAR(node->car);
		break;
	case TY_Cdr:
		CASE_CDR(node); //Need check
		break;
	case TY_Op:
		CASE_OP(node->car);
		break;
	case TY_Str:
		CASE_STR(node->car);
		break;
	case TY_Setq:
		CASE_SETQ(node->car);
		break;
	case TY_Defun:
		CASE_DEFUN(node->car);
		break;
	case TY_If:
		CASE_IF(node->car);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node->car);
	}

	printf("car2:type:%d:n:%d\n",at(list,n)->type,n);

	node->cdr = Cons_New();

	switch(at(list,n)->type) {
	case TY_Car:
		CASE_CAR(node->cdr);
		break;
	case TY_Cdr:
		CASE_CDR(node->cdr);
		break;
	case TY_Op:
		CASE_OP(node->cdr);
		break;
	case TY_Str:
		CASE_STR(node->cdr);
		break;
	case TY_Value:
		CASE_VALUE(node->cdr);
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
	node->type = TY_Op;
	node->svalue = at(list,n)->str;
	node->len = at(list,n)->size;
	node->cdr = Cons_New();
	node = node->cdr;
	++n;
	printf("op:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type) {
	case TY_Car:
		CASE_CAR(node);
		break;
	case TY_Cdr:
		CASE_CDR(node);
		break;
	case TY_Value:
		CASE_VALUE(node);
		break;
	case TY_Str:
		CASE_STR(node);
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
	printf("value:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type) {
	case TY_Car:
		CASE_CAR(node);
		break;
	case TY_Cdr:
		CASE_CDR(node);
		break;
	case TY_Value:
		CASE_VALUE(node);
		break;
	case TY_Str:
		CASE_STR(node);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

int F_Str(list_string_t *list, cons_t *node,int n)
{
	node->type = TY_Str;
	node->svalue = at(list,n)->str;
	node->len = at(list,n)->size;
	node->cdr = Cons_New();
	node = node->cdr;
	++n;
	printf("str:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type) {
	case TY_Car:
		CASE_CAR(node);
		break;
	case TY_Cdr:
		CASE_CDR(node);
		break;
	case TY_Value:
		CASE_VALUE(node);
		break;
	case TY_Str:
		CASE_STR(node);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

int F_Setq(list_string_t *list, cons_t *node,int n)
{
	node->type = TY_Setq;
	node->car = Cons_New();
	node->cdr = Cons_New();
	++n;
	printf("setq:type:%d:n:%d\n",at(list,n)->type,n);
	node->car->type = TY_Str;
	node->car->svalue = at(list,n)->str;
	node->car->len = at(list,n)->size;
	++n;

	switch(at(list,n)->type){
	case TY_Car:
		CASE_CAR(node->cdr);
		break;
	case TY_Value:
		CASE_VALUE(node->cdr);
		break;
	case TY_Str:
		CASE_STR(node->cdr);
		break;
	default:
		DEFAULT(node);
	}
	return n;
}

int F_If(list_string_t *list, cons_t *node,int n)
{
	node->type =TY_If;
	node->car = Cons_New();
	++n;
	printf("if:type:%d:n:%d\n",at(list,n)->type,n);
	cons_t *tmp = node->car;
	tmp->type = TY_Car;
	tmp->car = Cons_New();
	++n;
	printf("if2:type:%d:n:%d\n",at(list,n)->type,n);
	switch(at(list,n)->type) {
	case TY_Car:
		CASE_CAR(tmp->car);
		break;
	case TY_Cdr:
		CASE_CDR(tmp); //Need check
		break;
	case TY_Op:
		CASE_OP(tmp->car);
		break;
	case TY_Value:
		CASE_VALUE(tmp->car);
		break;
	case TY_Str:
		CASE_STR(tmp->car);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(tmp->car);
	}

	node->cdr = Cons_New();
	node->cdr->type = TY_Car;
	node->cdr->car = Cons_New();
	++n;
	printf("if3:type:%d:n:%d\n",at(list,n)->type,n);
	switch(at(list,n)->type) {
	case TY_Car:
		CASE_CAR(node->cdr->car);
		break;
	case TY_Cdr:
		CASE_CDR(node); //Need check
		break;
	case TY_Op:
		CASE_OP(node->cdr->car);
		break;
	case TY_Str:
		CASE_STR(node->cdr->car);
		break;
	case TY_Value:
		CASE_VALUE(node->cdr->car);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node->cdr->car);
	}
	node->cdr->cdr = Cons_New();
	printf("if4:type:%d:n:%d\n",at(list,n)->type,n);
	switch(at(list,n)->type) {
	case TY_Car:
		CASE_CAR(node->cdr->cdr);
		break;
	case TY_Cdr:
		CASE_CDR(node->cdr->cdr); //Need check
		break;
	case TY_Op:
		CASE_OP(node->cdr->cdr);
		break;
	case TY_Str:
		CASE_STR(node->cdr->cdr);
		break;
	case TY_Value:
		CASE_VALUE(node->cdr->cdr);
		break;
	case TY_EOL:
		break;
	default:
		DEFAULT(node->cdr);
	}
	return n;
}

int F_Defun(list_string_t *list, cons_t *node,int n)
{
	node->type = TY_Defun;
	node->car = Cons_New();
	node->cdr = Cons_New();
	++n;
	printf("defun:type:%d:n:%d\n",at(list,n)->type,n);
	node->car->type = TY_Str;
	node->car->svalue = at(list,n)->str;
	node->car->len = at(list,n)->size;
	node->car->cdr = Cons_New();
	++n;
	node->car->cdr->type = TY_Car;
	node->car->cdr->car = Cons_New();
	++n;
	cons_t *tmp = node->car->cdr->car;
	while(at(list,n)->type != TY_Cdr) {
		tmp->type = TY_Str;
		tmp->svalue = at(list,n)->str;
		tmp->len = at(list,n)->size;
		tmp->cdr = Cons_New();
		tmp = tmp->cdr;
		++n;
	}
	tmp->type = TY_Cdr;
	tmp->svalue = ")";
	tmp->len = 1;
	++n;

	switch(at(list,n)->type){
	case TY_Car:
		CASE_CAR(node->cdr);
		break;
	case TY_Value:
		CASE_VALUE(node->cdr);
		break;
	case TY_Str:
		CASE_STR(node->cdr);
		break;
	default:
		DEFAULT(node);
	}

	return n;
}
