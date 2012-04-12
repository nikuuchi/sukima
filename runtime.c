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
list_run_t *asm_If(list_run_t *cmd,cons_t *cu);
list_run_t *asm_Defun(list_run_t *cmd,cons_t *cu,st_table_t *hash);
list_run_t *asm_Op(list_run_t *cmd,cons_t *cu);
list_run_t *asm_UseFunction(list_run_t *cmd,cons_t *cu);
list_run_t *assemble(list_run_t *p,cons_t *cu,st_table_t *hash);

list_run_t *ListRun_New()
{
	list_run_t *list_run_t_ptr = (list_run_t *)malloc(sizeof(list_run_t));
	list_run_t_ptr->v = (value_t *)malloc(sizeof(value_t));
	return list_run_t_ptr;
}


void freeListRun(list_run_t *p)
{
	if(p != NULL) {
		if(p->v->type == Pointer){
			free(p->v->svalue);
		}
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
			p = assemble(p,chain->car,hash);
			break;
		case TY_Value:
			setValue(p,chain->ivalue);
			break;
		default:
			printf("some error occured in run().\n");
			break;
		}
		chain = chain->cdr;
	}
	p->command = C_End;
}

list_run_t *assemble(list_run_t *p,cons_t *cu,st_table_t *hash)
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
	case TY_Defun:
		p = asm_Defun(p,cu,hash);
		break;
	case TY_If:
		p = asm_If(p,cu);
		break;
	default:
		printf("some error occured in assemble().\n");
		break;
	}
	return p;
}

list_run_t *asm_Car(list_run_t *cmd,cons_t *cu)
{
	if(cu->car->type == TY_Str) {
		return asm_UseFunction(cmd,cu->car);
	}else if(cu->car->type == TY_If) {
		return asm_If(cmd,cu->car);
	}
	return asm_Op(cmd,cu->car);
}

list_run_t *asm_Defun(list_run_t *cmd,cons_t *cu,st_table_t *hash)
{
	list_run_t *func = ListRun_New();
	list_run_t *list = func;

	cons_t *tmp = cu->cdr->cdr->car;
	while(tmp->type != TY_Cdr) {
		list->command = C_PutObject;
		list->v->type = Pointer;
		list->v->svalue = tmp->svalue;
		list->v->len = tmp->len;

		list->next = ListRun_New();
		list = list->next;
		tmp = tmp->cdr;
	}
	list = asm_Car(list,cu->cdr->cdr->cdr);
	list->command = C_End;
	HashTable_insert_Function(hash,cu->cdr->svalue,cu->cdr->len,func);

	return cmd;
}

char *createTag()
{
	static char *createTagCharacters[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z","0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
	int b = 60;
	char *str = (char *)malloc(sizeof(char) * 8);
	int i=0;
	static int createTagFlag;
	if(createTagFlag == 0){
		srand( (unsigned)time(NULL));
		createTagFlag = 1;
	}
	for(i =0;i<8;++i) {
		strcat(str,createTagCharacters[rand() % b]);
	}
	return str;
}

list_run_t *asm_If(list_run_t *cmd,cons_t *cu)
{
	list_run_t *list = cmd;
	
	//condition
	list = asm_Car(list,cu->cdr);

	//tag jump
	char *tag = createTag();
	list->command = C_TJump;
	list->v->type = Pointer;
	list->v->svalue = tag;
	list->v->len = 8;
	list->next = ListRun_New();
	list = list->next;

	//if true
	list = asm_Car(list,cu->cdr->cdr);

	//end jump
	char *end_tag = createTag();
	list->command = C_Jump;
	list->v->type = Pointer;
	list->v->svalue = end_tag;
	list->v->len = 8;
	list->next = ListRun_New();
	list = list->next;

	//tag
	list->command = C_Tag;
	list->v->type = Pointer;
	list->v->svalue = tag;
	list->v->len = 8;
	list->next = ListRun_New();
	list = list->next;

	//if false
	list = asm_Car(list,cu->cdr->cdr->cdr);

	//end_tag
	list->command = C_Tag;
	list->v->type = Pointer;
	list->v->svalue = end_tag;
	list->v->len = 8;
	list->next = ListRun_New();
	list = list->next;
	return list;
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
		printf("some error occured in asm_Setq().\n");
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
			printf("some error occured in asm_Op() 1.\n");
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
		printf("some error occured in asm_Op() 2. %s\n",cu->svalue);
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
			printf("some error occured in asm_UseFunction().\n");
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
