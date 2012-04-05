#include "lisp.h"

#define SWITCH switch(at(list,n)->type)
#define CASE_CAR(c)						\
	case TY_Car:						\
	n = F_Car(list,(c),n);				\
	break;

#define CASE_CDR(c)								\
	case TY_Cdr:								\
	(c)->type = TY_Cdr;							\
	   (c)->svalue = "Nil";						\
		  ++n;									\
		  break;
#define CASE_OP(c)								\
	case TY_Op:									\
	n = F_Op(list,(c),n);						\
	break;

#define CASE_VALUE(c) case TY_Value:			\
	n = F_Value(list,(c),n);					\
	break;

#define CASE_EOL()								\
	case TY_EOL:								\
	break;


#define DEFAULT(c)\
	default:									\
	F_ERROR(list,(c),n);

#define Cons_New() (cons_t *)malloc(sizeof(cons_t))

void F_ERROR(list_string *list, cons_t *node,int n);
int F_Car(list_string *list, cons_t *node,int n);
int F_Op(list_string *list, cons_t *node, int n);
int F_Cdr(list_string *list, cons_t *node,int n);
int F_Value(list_string *list, cons_t *node,int n);

list_string *at(list_string *list,int n){
	list_string *p = list;
	for(int i=0;i<n;++i){
		p = p->next;
	}
	return p;
}

void parse(list_string *list, cons_t *node)
{
//	printf("car:%d:cdr:%d:op:%d:value:%d:str:%d\n",TY_Car,TY_Cdr,TY_Op,TY_Value,TY_Str);
	int n = 0;
//	printf("type:%d:n:%d\n",at(list,n)->type,n);
	while(at(list,n)->type != TY_EOL){
		SWITCH{
			CASE_CAR(node);
			DEFAULT(node);
		}
	}
}

void F_ERROR(list_string *list, cons_t *node,int n)
{
	printf("parse error. %s \n",at(list,n)->str);
}

int F_Car(list_string *list, cons_t *node,int n)
{
	node->type = TY_Car;
	node->car = Cons_New();
	++n;
//	printf("car1:type:%d:n:%d\n",at(list,n)->type,n);

	SWITCH{
		CASE_CAR(node->car);
		CASE_CDR(node->car);
		CASE_OP(node->car);
		CASE_EOL();
		DEFAULT(node->car);
	}

//		printf("car2:type:%d:n:%d\n",at(list,n)->type,n);
	if(at(list,n)->type != TY_EOL){
		node->cdr = (cons_t *)malloc(sizeof(cons_t));
	}
	SWITCH{
		CASE_CAR(node->cdr);
		CASE_CDR(node->cdr);
		CASE_OP(node->cdr);
		CASE_VALUE(node->cdr);
		CASE_EOL();
		DEFAULT(node->cdr);
	}
	return n;
}


int F_Op(list_string *list, cons_t *node,int n)
{
	node->type = TY_Op;
	node->svalue = at(list,n)->str;
	node->cdr = Cons_New();
	node = node->cdr;
	++n;
//	printf("op:type:%d:n:%d\n",at(list,n)->type,n);

	SWITCH{
		CASE_CAR(node);
		CASE_CDR(node);
		CASE_VALUE(node);
		CASE_EOL();
		DEFAULT(node);
	}
	return n;
}



int F_Value(list_string *list, cons_t *node,int n)
{
	node->type = TY_Value;
	node->ivalue = atoi(at(list,n)->str);
	node->cdr = Cons_New();
	++n;
	node = node->cdr;
//	printf("value:type:%d:n:%d\n",at(list,n)->type,n);

	SWITCH{
		CASE_CAR(node);
		CASE_CDR(node);
		CASE_VALUE(node);
		CASE_EOL();
		DEFAULT(node);
	}
	return n;
}
