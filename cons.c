#include "lisp.h"

void freeCons_t(cons_t * p)
{
	if(p->car != NULL && p->type == TY_Car)
		freeCons_t(p->car);
	if(p->cdr != NULL)
		freeCons_t(p->cdr);
	free(p);
}

void dumpCons_t(cons_t * p)
{
	if(p != NULL){
		if(p->type == TY_Car){
			printf("( ");
			dumpCons_t(p->car);
		}else if(p->type == TY_Op){
			printf("%s ",p->svalue);
		}else if(p->type == TY_Value){
			printf("%d ",p->ivalue);
		}else if(p->type == TY_Cdr){
			printf(") ");
		}
		if(p->cdr != NULL){
			dumpCons_t(p->cdr);
		}
	}
}

