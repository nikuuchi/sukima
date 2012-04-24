#include "lisp.h"

void freeCons_t(cons_t * p)
{
	if(p->car != NULL && p->type == TY_Car)
		freeCons_t(p->car);
	if(p->cdr != NULL)
		freeCons_t(p->cdr);
	free(p);
	p = NULL;
}

void dumpCons_t(cons_t * p)
{
	if(p != NULL){
		switch(p->type) {
		case TY_Car:
			printf("( ");
			dumpCons_t(p->car);
			break;
		case TY_Op:
		case TY_Str:
			printf("%s ",p->string.s);
			break;
		case TY_Int:
			printf("%d ",p->ivalue);
			break;
		case TY_Float:
			printf("%f ",p->fvalue);
			break;
		case TY_Cdr:
			printf(") ");
			break;
		case TY_If:
			printf("if ");
			break;
		case TY_Setq:
			printf("setq ");
			break;
		case TY_Defun:
			printf("defun ");
			break;
		case TY_EOL:
			printf("EOL ");
			break;
		default:
			printf("Not implemented yet.");
		}

		if(p->cdr != NULL){
			dumpCons_t(p->cdr);
		}
	}
}

