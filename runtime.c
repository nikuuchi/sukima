#include "lisp.h"

#define setValue(c,d) \
	do { \
		(c)->command = C_Put; \
		(c)->v->type = Integer; \
		(c)->v->num = (d); \
	} while(0);

#define setStr(c,d,e)							\
	do { \
		(c)->command = C_LoadValue; \
		(c)->v->type = Pointer; \
		(c)->v->svalue = (d); \
		(c)->v->len = (e); \
	} while(0);

#define setBoolean(c,d) \
	do { \
		(c)->command = C_Put; \
		(c)->v->type = Boolean; \
		(c)->v->svalue = (d); \
	} while(0);

#define setOpt(c,d,e) \
	do { \
		(c)->command = (d); \
		(c)->v->type = Integer; \
		(c)->v->num = (e); \
	} while(0);

list_run_t *asm_Car(list_run_t *cmd,cons_t *cu);
list_run_t *asm_Setq(list_run_t *cmd,cons_t *cu);
list_run_t *asm_Op(list_run_t *cmd,cons_t *cu);
list_run_t *asm_UseFunction(list_run_t *cmd,cons_t *cu);
list_run_t *assemble(list_run_t *p,cons_t *cu);

list_run_t *ListRun_New()
{
	list_run_t *list_run_t_ptr = (list_run_t *)malloc(sizeof(list_run_t));
	list_run_t_ptr->v = (value_t *)malloc(sizeof(value_t));
	return list_run_t_ptr;
}


void freeListRun(list_run_t *p)
{
	if(p != NULL) {
		free(p->v);
		freeListRun(p->next);
	}
	free(p);
}

void compile(cons_t *ast,list_run_t *root,st_table_t *hash)
{
	cons_t *chain = ast;
	list_run_t *p = root;
	while(chain->cdr != NULL){
		switch(chain->type){
		case TY_Car:
			p = assemble(p,chain->car);
			break;
		case TY_Value:
			setValue(p,chain->ivalue);
			break;
		default:
			printf("some error occured in run()->\n");
			break;			
		}
		chain = chain->cdr;
	}
	p->command = C_End;
}

list_run_t *assemble(list_run_t *p,cons_t *cu)
{
	switch(cu->type) {
	case TY_Cdr:
		setBoolean(p,"Nil");
		break;
	case TY_Op:
		p = asm_Op(p,cu);
		break;
	case TY_EOL:
		p->command = C_End;
		break;
	case TY_Str:
		p = asm_UseFunction(p,cu);
		break;
	case TY_Setq:
		p = asm_Setq(p,cu);
		break;
	default:
		printf("some error occured in assemble()->\n");
		break;
	}
	return p;
}

list_run_t *asm_Car(list_run_t *cmd,cons_t *cu)
{
	return asm_Op(cmd,cu->car);
}

list_run_t *asm_Setq(list_run_t *cmd,cons_t *cu)
{
	cons_t *p = cu->cdr;
	list_run_t *list = cmd;

	switch(p->type) {
	case TY_Car:
		list = asm_Car(list,p);
		break;
	case TY_Value:
		setValue(list,p->ivalue);
		list->next = ListRun_New();
		list = list->next;
		break;
	default:
		printf("some error occured in asm_Setq()->\n");
		break;
	}
	


	list->command = C_PutObject;
	list->v->type = Pointer;
	list->v->svalue = cu->car->svalue;
	list->v->len = cu->car->len;

	list->next = ListRun_New();
	list = list->next;
	return list;
}

list_run_t *asm_Op(list_run_t *cmd,cons_t *cu)
{
	int count = 0;
	cons_t *p = cu->cdr;
	list_run_t *list = cmd;

	while(p->type != TY_Cdr) {
		switch(p->type) {
		case TY_Car:
			list = asm_Car(list,p);
			break;
		case TY_Value:
			setValue(list,p->ivalue);
			list->next = ListRun_New();
			list = list->next;
			break;
		case TY_Str:
			setStr(list,p->svalue,p->len);
			list->next = ListRun_New();
			list = list->next;
			break;
		default:
			printf("some error occured in asm_Op()->\n");
			break;
		}
		++count;
		p = p->cdr;
	}

	switch(cu->svalue[0]) {
	case '+':
		setOpt(list,C_OptPlus,count);
		break;
	case '-':
		setOpt(list,C_OptMinus,count);
		break;
	case '*':
		setOpt(list,C_OptMul,count);
		break;
	case '/':
		setOpt(list,C_OptDiv,count);
		break;
	case '<':
		setOpt(list,C_OptLt,count);
		break;
	case '>':
		setOpt(list,C_OptGt,count);
		break;
	default:
		printf("some error occured in asm_Op()->\n");
		break;
	}
	list->next = ListRun_New();
	list = list->next;
	return list;
}

list_run_t *asm_UseFunction(list_run_t *cmd,cons_t *cu)
{
	int count = 0;
	cons_t *p = cu->cdr;
	list_run_t *list = cmd;

	while(p->type != TY_Cdr) {
		switch(p->type) {
		case TY_Car:
			list = asm_Car(list,p);
			break;
		case TY_Value:
			setValue(list,p->ivalue);
			list->next = ListRun_New();
			list = list->next;
			break;
		case TY_Str:
			setStr(list,p->svalue,p->len);
			list->next = ListRun_New();
			list = list->next;
			break;
		default:
			printf("some error occured in asm_UseFunction()->\n");
			break;
		}
		++count;
		p = p->cdr;
	}

	if(strcmp(cu->svalue,"print") == 0) {
		list->command = C_Print;
		list->v->type = CALLFUNCTION;
		list->v->svalue = "print";
		list->v->len = strlen("print");
	}else{
		list->command = C_Call;
		list->v->type = CALLFUNCTION;
		list->v->num = count;
		list->v->svalue = cu->svalue;
		list->v->len = cu->len;
	}

	list->next = ListRun_New();
	list = list->next;
	return list;
}
