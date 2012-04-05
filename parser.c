#include "lisp.h"

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
	printf("car:%d:cdr:%d:op:%d:value:%d:str:%d\n",TY_Car,TY_Cdr,TY_Op,TY_Value,TY_Str);
	int n = 0;
	printf("type:%d:n:%d\n",at(list,n)->type,n);
	while(at(list,n)->type != TY_EOL){
		switch(at(list,n)->type){
		case TY_Car:
			n = F_Car(list,node,n);
			break;
		default:
			F_ERROR(list,node,n);
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
	node->car = (cons_t *)malloc(sizeof(cons_t));
	++n;
	printf("car1:type:%d:n:%d\n",at(list,n)->type,n);

	switch(at(list,n)->type){
	case TY_Car:
		n = F_Car(list,node->car,n);
		break;
	case TY_Cdr:
		node->type = TY_Cdr;
		node->svalue = "Nil";
		++n;
		break;
	case TY_Op:
		n = F_Op(list,node->car,n);
		break;
	case TY_EOL:
		break;
	default:
		F_ERROR(list,node->car,n);
	}

	if(at(list,n)->type != TY_EOL){
		printf("car2:type:%d:n:%d\n",at(list,n)->type,n);
		node->cdr = (cons_t *)malloc(sizeof(cons_t));

		switch(at(list,n)->type){
		case TY_Car:
			n = F_Car(list,node->cdr,n);
			break;
		case TY_Cdr:
			node->cdr->type = TY_Cdr;
			node->cdr->svalue = "Nil";
			++n;
			break;
		case TY_Op:
			n = F_Op(list,node->cdr,n);
			break;
		case TY_EOL:
			break;
		case TY_Value:
			n = F_Value(list,node->cdr,n);
			break;
		default:
			F_ERROR(list,node->cdr,n);
		}
	}
	return n;
}


int F_Op(list_string *list, cons_t *node,int n)
{
	node->type = TY_Op;
	node->svalue = at(list,n)->str;
	node->cdr = (cons_t *)malloc(sizeof(cons_t));
	node = node->cdr;
	++n;
	printf("op:type:%d:n:%d\n",at(list,n)->type,n);

    if(at(list,n)->type != TY_EOL){
		switch(at(list,n)->type){
		case TY_Car:
			n = F_Car(list,node,n);
			break;
		case TY_Cdr:
			node->type = TY_Cdr;
			node->svalue = "Nil";
			++n;
			break;
		case TY_Value:
			n = F_Value(list,node,n);
			break;
		case TY_EOL:
			break;
		default:
			F_ERROR(list,node,n);
		}
	}
	return n;
}



int F_Value(list_string *list, cons_t *node,int n)
{
	node->type = TY_Value;
	node->ivalue = atoi(at(list,n)->str);
	node->cdr = (cons_t *)malloc(sizeof(cons_t));
	++n;
	node = node->cdr;
	printf("value:type:%d:n:%d\n",at(list,n)->type,n);

    if(at(list,n)->type != TY_EOL){
		switch(at(list,n)->type){
		case TY_Car:
			n = F_Car(list,node,n);
			break;
		case TY_Cdr:
			node->type = TY_Cdr;
			node->svalue = "Nil";
			++n;
			break;
		case TY_Value:
			n = F_Value(list,node,n);
			break;
		case TY_EOL:
			break;
		default:
			F_ERROR(list,node,n);
		}
	}
	return n;
}
