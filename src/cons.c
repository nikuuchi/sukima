#include "sukima.h"

void freeCons_t(cons_t * p)
{
	switch(p->type) {
	case TY_Op:
	case TY_Str:
	case TY_CStr:
	case TY_If:
	case TY_Setq:
	case TY_Defun:
		free(p->string.s);
		break;
	default:
		;
	}

	if(p->car != NULL && p->type == TY_LParen)
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
		case TY_LParen:
			printf("( ");
			dumpCons_t(p->car);
			break;
		case TY_Op:
		case TY_Str:
			printf("%s ",p->string.s);
			break;
		case TY_CStr:
			printf("%s ",p->string.s);
			break;
		case TY_Int:
			printf("%d ",p->ivalue);
			break;
		case TY_Double:
			printf("%f ",p->fvalue);
			break;
		case TY_RParen:
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
		case TY_Boolean:
			if(p->ivalue)
				printf("T ");
			else
				printf("NIL ");
			break;
		case TY_EOL:
			printf(" EOL");
			break;
		default:
			printf("Not implemented yet.");
		}

		if(p->cdr != NULL){
			dumpCons_t(p->cdr);
		}
	}
}

