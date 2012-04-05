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
	while(at(list,n)->type != TY_EOL){
		switch(at(list,n)->type){
		case TY_Car:
			printf("type:%d:n:%d\n",at(list,n)->type,n);
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

	switch(at(list,n)->type){
	case TY_Car:
		printf("car1:type:%d:n:%d\n",at(list,n)->type,n);
		n = F_Car(list,node->car,n);
		break;
	case TY_Cdr:
		printf("car1:type:%d:n:%d\n",at(list,n)->type,n);
		node->type = TY_Cdr;
		node->svalue = "Nil";
		++n;
		break;
	case TY_Op:
		printf("car1:type:%d:n:%d\n",at(list,n)->type,n);
		n = F_Op(list,node->car,n);
		break;
	case TY_EOL:
		break;
	default:
		F_ERROR(list,node->car,n);
	}

	if(at(list,n)->type != TY_EOL){
		node->cdr = (cons_t *)malloc(sizeof(cons_t));

		switch(at(list,n)->type){
		case TY_Car:
			printf("car2:type:%d:n:%d\n",at(list,n)->type,n);
			n = F_Car(list,node->cdr,n);
			break;
		case TY_Cdr:
			printf("car2:type:%d:n:%d\n",at(list,n)->type,n);
			if(at(list,n+1)->type == TY_EOL){
				printf("--a--\n");
				node->cdr->type = TY_Cdr;
				node->cdr->svalue = "Nil";
			}
			++n;
			break;
		case TY_Op:
			printf("car2:type:%d:n:%d\n",at(list,n)->type,n);
			n = F_Op(list,node->cdr,n);
			break;
		case TY_EOL:
			break;
		case TY_Value:
			printf("car2:type:%d:n:%d\n",at(list,n)->type,n);
			n = F_Value(list,node->cdr,n);
			node = node->cdr;
			node->cdr = (cons_t *)malloc(sizeof(cons_t));
			node->cdr->type = TY_Cdr;
			node->cdr->svalue = "Nil";
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

	int depth = 1;
	while(depth  > 0 && at(list,n)->type != TY_EOL){
		switch(at(list,n)->type){
		case TY_Car:
			printf("op:type:%d:n:%d\n",at(list,n)->type,n);
			n = F_Car(list,node,n);
			break;
		case TY_Cdr:
			printf("op:type:%d:n:%d\n",at(list,n)->type,n);
			--depth;
			++n;
			break;
		case TY_Value:
			printf("op:type:%d:n:%d\n",at(list,n)->type,n);
			n = F_Value(list,node,n);
			node->cdr->type = TY_Cdr;
			node->cdr->svalue = "Nil";
			node = node->cdr;
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
	return n;
}
