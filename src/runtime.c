#include "lisp.h"

#define setValue(c,d) \
	do { \
		(c)->command = C_Put; \
		(c)->v->type = Integer; \
		(c)->v->num = (d); \
		(c)->iseq = tables[C_Put]; \
	} while(0);

#define setStr(c,d,e) \
	do { \
		(c)->command = C_LoadValue; \
		(c)->v->type = Pointer; \
		(c)->v->string.s = (d); \
		(c)->v->string.len = (e); \
		(c)->iseq = tables[C_LoadValue]; \
	} while(0);

#define setBoolean(c,d) \
	do { \
		(c)->command = C_Put; \
		(c)->v->type = Boolean; \
		(c)->v->string.s = (d); \
		(c)->iseq = tables[C_Put]; \
	} while(0);

#define setOpt(c,d,e) \
	do { \
		(c)->command = (d); \
		(c)->v->type = Integer; \
		(c)->v->num = (e); \
		(c)->iseq = tables[(d)]; \
	} while(0);

void **tables;
list_run_t *asm_Car(list_run_t *cmd,cons_t *cu,st_table_t *hash);
list_run_t *asm_Setq(list_run_t *cmd,cons_t *cu,st_table_t *hash);
list_run_t *asm_If(list_run_t *cmd,cons_t *cu,st_table_t *hash);
list_run_t *asm_Defun(list_run_t *cmd,cons_t *cu,st_table_t *hash);
list_run_t *asm_Op(list_run_t *cmd,cons_t *cu,st_table_t *hash);
list_run_t *asm_UseFunction(list_run_t *cmd,cons_t *cu,st_table_t *hash);
list_run_t *assemble(list_run_t *p,cons_t *cu,st_table_t *hash);
list_run_t *asm_DefunUseFunction(list_run_t *cmd,cons_t *cu, st_table_t *argument,st_table_t *hash);
list_run_t *asm_DefunOp(list_run_t *cmd,cons_t *cu, st_table_t *argument,st_table_t *hash);
list_run_t *asm_DefunIf(list_run_t *cmd,cons_t *cu, st_table_t *argument,st_table_t *hash);

list_run_t *ListRun_New()
{
	list_run_t *list_run_t_ptr = (list_run_t *)malloc(sizeof(list_run_t));
	memset(list_run_t_ptr,0x00,sizeof(list_run_t));
	list_run_t_ptr->v = (value_t *)malloc(sizeof(value_t));
	return list_run_t_ptr;
}

void freeListRun(list_run_t *p)
{
	if(p != NULL) {
		if(p->v->type == Pointer){
			free(p->v->string.s);
		}
		free(p->v);
		freeListRun(p->next);
	}
	free(p);
	p = NULL;
}

void compile(cons_t *ast,list_run_t *root,st_table_t *hash)
{
	cons_t *chain = ast;
	list_run_t *p = root;
	value_t **st = (value_t **)malloc(sizeof(value_t));
	tables = vm_exec(root,st,0,hash,1);
	free(st);
	while(chain->cdr != NULL){
		switch(chain->type){
		case TY_Car:
			p = assemble(p,chain->car,hash);
			break;
		case TY_Value:
			setValue(p,chain->ivalue);
			break;
		default:
			printf("some error occured in compile().\n");
			break;
		}
		chain = chain->cdr;
	}
	p->command = C_End;
	p->iseq = tables[C_End];
}

list_run_t *assemble(list_run_t *p,cons_t *cu,st_table_t *hash)
{
	switch(cu->type) {
	case TY_Cdr:
		setBoolean(p,"Nil");
		break;
	case TY_Op:
		p = asm_Op(p,cu,hash);
		break;
	case TY_EOL:
		p->command = C_End;
		break;
	case TY_Str:
		p = asm_UseFunction(p,cu,hash);
		break;
	case TY_Setq:
		p = asm_Setq(p,cu,hash);
		break;
	case TY_Defun:
		p = asm_Defun(p,cu,hash);
		break;
	case TY_If:
		p = asm_If(p,cu,hash);
		break;
	default:
		printf("some error occured in assemble().\n");
		break;
	}
	return p;
}

list_run_t *asm_Car(list_run_t *cmd,cons_t *cu,st_table_t *hash)
{
	if(cu->car->type == TY_Str) {
		return asm_UseFunction(cmd,cu->car,hash);
	}else if(cu->car->type == TY_If) {
		return asm_If(cmd,cu->car,hash);
	}
	return asm_Op(cmd,cu->car,hash);
}

list_run_t *asm_DefunCar(list_run_t *cmd,cons_t *cu,st_table_t *argument,st_table_t *hash)
{
	if(cu->car->type == TY_Str) {
		return asm_DefunUseFunction(cmd,cu->car,argument,hash);
	}else if(cu->car->type == TY_If) {
		return asm_DefunIf(cmd,cu->car,argument,hash);
	}
	return asm_DefunOp(cmd,cu->car,argument,hash);
}

list_run_t *asm_Defun(list_run_t *cmd,cons_t *cu,st_table_t *hash)
{
	list_run_t *func = ListRun_New();
	HashTable_insert_Function(hash,cu->cdr->string.s,cu->cdr->string.len,func);
	list_run_t *list = func;

	st_table_t *argument = HashTable_init();

	cons_t *tmp = cu->cdr->cdr->car;
	int i = 1;
	while(tmp->type != TY_Cdr) {
		value_t *v = (value_t *)malloc(sizeof(value_t));
		v->num = i;
		HashTable_insert_Value(argument,tmp->string.s,tmp->string.len,v);
		tmp = tmp->cdr;
		++i;
	}
	list = asm_DefunCar(list,cu->cdr->cdr->cdr,argument,hash);
	list->command = C_End;
	list->iseq = tables[C_End];
	

	HashTable_free(argument);
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

list_run_t *asm_If(list_run_t *cmd,cons_t *cu, st_table_t *hash)
{
	list_run_t *list = cmd;
	//condition
	list = asm_Car(list,cu->cdr,hash);

	//tag jump
	char *tag = createTag();
	list->command = C_TJump;
	list->v->type = Pointer;
	list->v->string.s = tag;
	list->v->string.len = 8;
	list->iseq = tables[C_TJump];
	list = ListRun_New();
	list = list->next;

	//if true
	list = asm_Car(list,cu->cdr->cdr,hash);

	//end jump
	char *end_tag = createTag();
	list->command = C_Jump;
	list->v->type = Pointer;
	list->v->string.s = end_tag;
	list->v->string.len = 8;
	list->iseq = tables[C_Jump];
	list->next = ListRun_New();
	list = list->next;

	//tag
	list->command = C_Tag;
	list->v->type = Pointer;
	list->v->string.s = tag;
	list->v->string.len = 8;
	list->iseq = tables[C_Tag];
	list->next = ListRun_New();
	list = list->next;

	//if false
	list = asm_Car(list,cu->cdr->cdr->cdr,hash);

	//end_tag
	list->command = C_Tag;
    list->v->type = Pointer;
	list->v->string.s = end_tag;
	list->v->string.len = 8;
	list->iseq = tables[C_Tag];
	list->next = ListRun_New();
	list = list->next;

	return list;
}

list_run_t *asm_Setq(list_run_t *cmd,cons_t *cu, st_table_t *hash)
{
	cons_t *p = cu->cdr->cdr;
	list_run_t *list = cmd;

	switch(p->type) {
	case TY_Car:
		list = asm_Car(list,p,hash);
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
	list->v->string.s = cu->cdr->string.s;
	list->v->string.len = cu->cdr->string.len;
	list->iseq = tables[C_PutObject];

	list->next = ListRun_New();
	list = list->next;
	return list;
}

list_run_t *asm_Op(list_run_t *cmd,cons_t *cu,st_table_t *hash)
{
	int count = 0;
	cons_t *p = cu->cdr;
	list_run_t *list = cmd;

	while(p->type != TY_Cdr) {
		switch(p->type) {
		case TY_Car:
			list = asm_Car(list,p,hash);
			break;
		case TY_Value:
			setValue(list,p->ivalue);
			list->next = ListRun_New();
			list = list->next;
			break;
		case TY_Str:
			setStr(list,p->string.s,p->string.len);
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

	switch(cu->string.s[0]) {
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
		printf("some error occured in asm_Op() 2. %s\n",cu->string.s);
		break;
	}
	list->next = ListRun_New();
	list = list->next;
	return list;
}

list_run_t *asm_UseFunction(list_run_t *cmd,cons_t *cu,st_table_t *hash)
{
	int count = 0;
	cons_t *p = cu->cdr;
	list_run_t *list = cmd;

	while(p->type != TY_Cdr) {
		switch(p->type) {
		case TY_Car:
			list = asm_Car(list,p,hash);
			break;
		case TY_Value:
			setValue(list,p->ivalue);
			list->next = ListRun_New();
			list = list->next;
			break;
		case TY_Str:
			setStr(list,p->string.s,p->string.len);
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

	if(strcmp(cu->string.s,"print") == 0) {
		list->command = C_Print;
		list->v->type = CALLFUNCTION;
		list->v->string.s = "print";
		list->v->string.len = strlen("print");
		list->iseq = tables[C_Print];
	}else{
		list->command = C_Call;
		list->v->type = CALLFUNCTION;
		list->v->num = count;
		list->v->string.s = cu->string.s;
		list->v->string.len = cu->string.len;
		list->v->func = HashTable_lookup_Function(hash, cu->string.s,cu->string.len);
		list->iseq = tables[C_Call];
	}

	list->next = ListRun_New();
	list = list->next;
	return list;
}

list_run_t *asm_DefunUseFunction(list_run_t *cmd,cons_t *cu,st_table_t *argument, st_table_t *hash)
{
	int count = 0;
	cons_t *p = cu->cdr;
	list_run_t *list = cmd;

	while(p->type != TY_Cdr) {
		switch(p->type) {
		case TY_Car:
			list = asm_DefunCar(list,p,argument,hash);
			break;
		case TY_Value:
			setValue(list,p->ivalue);
			list->next = ListRun_New();
			list = list->next;
			break;
		case TY_Str: {
			value_t *v = HashTable_lookup_Value(argument,p->string.s,p->string.len);
			list->command = C_Args;
			list->v->type = Integer;
			list->v->num = v->num;
			list->iseq = tables[C_Args];
			list->next = ListRun_New();
			list = list->next;
			break;
		}
		default:
			printf("some error occured in asm_UseFunction().\n");
			break;
		}
		++count;
		p = p->cdr;
	}

	if(strcmp(cu->string.s,"print") == 0) {
		list->command = C_Print;
		list->v->type = CALLFUNCTION;
		list->v->string.s = "print";
		list->v->string.len = strlen("print");
		list->iseq = tables[C_Print];
	}else{
		list->command = C_Call;
		list->v->type = CALLFUNCTION;
		list->v->num = count;
		list->v->string.s = cu->string.s;
		list->v->string.len = cu->string.len;
		list->v->func = HashTable_lookup_Function(hash, cu->string.s,cu->string.len);
		list->iseq = tables[C_Call];
	}

	list->next = ListRun_New();
	list = list->next;
	return list;
}

list_run_t *asm_DefunOp(list_run_t *cmd,cons_t *cu, st_table_t *argument, st_table_t *hash)
{
	int count = 0;
	cons_t *p = cu->cdr;
	list_run_t *list = cmd;

	while(p->type != TY_Cdr) {
		switch(p->type) {
		case TY_Car:
			list = asm_DefunCar(list,p,argument,hash);
			break;
		case TY_Value:
			setValue(list,p->ivalue);
			list->next = ListRun_New();
			list = list->next;
			break;
		case TY_Str:{
			value_t *v = HashTable_lookup_Value(argument,p->string.s,p->string.len);
			list->command = C_Args;
			list->v->type = Integer;
			list->v->num = v->num;
			list->iseq = tables[C_Args];
			list->next = ListRun_New();
			list = list->next;
			break;
		}
		default:
			printf("some error occured in asm_Op() 1.\n");
			break;
		}
		++count;
		p = p->cdr;
	}

	switch(cu->string.s[0]) {
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
		printf("some error occured in asm_Op() 2. %s\n",cu->string.s);
		break;
	}
	list->next = ListRun_New();
	list = list->next;
	return list;
}

list_run_t *asm_DefunIf(list_run_t *cmd,cons_t *cu, st_table_t *argument,st_table_t *hash)
{
	list_run_t *list = cmd;
	
	//condition
	list = asm_DefunCar(list,cu->cdr,argument,hash);

	//tag jump
	char *tag = createTag();
	list->command = C_TJump;
	list->v->type = Pointer;
	list->v->string.s = tag;
	list->v->string.len = 8;
	list->iseq = tables[C_TJump];
	list->next = ListRun_New();
	list = list->next;

	//if true
	if(cu->cdr->cdr->type == TY_Car) {
		list = asm_DefunCar(list,cu->cdr->cdr,argument,hash);
	}else if(cu->cdr->cdr->type == TY_Value) {
		setValue(list,cu->cdr->cdr->ivalue);
		list->next = ListRun_New();
		list = list->next;
	}

	//end jump
	char *end_tag = createTag();
	list->command = C_Jump;
	list->v->type = Pointer;
	list->v->string.s = end_tag;
	list->v->string.len = 8;
	list->iseq = tables[C_Jump];
	list->next = ListRun_New();
	list = list->next;

	//tag
	list->command = C_Tag;
	list->v->type = Pointer;
	list->v->string.s = tag;
	list->v->string.len = 8;
	list->iseq = tables[C_Tag];
	list->next = ListRun_New();
	list = list->next;

	//if false
	if(cu->cdr->cdr->cdr->type == TY_Car) {
		list = asm_DefunCar(list,cu->cdr->cdr->cdr,argument,hash);
	}else if(cu->cdr->cdr->cdr->type == TY_Value) {
		setValue(list,cu->cdr->cdr->cdr->ivalue);
		list->next = ListRun_New();
		list = list->next;
	}
	//end_tag
	list->command = C_Tag;
	list->v->type = Pointer;
	list->v->string.s = end_tag;
	list->v->string.len = 8;
	list->iseq = tables[C_Tag];
	list->next = ListRun_New();
	list = list->next;
	return list;
}
