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

#define setOp(c,d,e) \
	do { \
		(c)->command = (d); \
		(c)->v->type = Integer; \
		(c)->v->num = (e); \
		(c)->iseq = tables[(d)]; \
	} while(0);

void **tables;
command_t *asm_Car(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_Setq(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_If(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_Defun(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_Op(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_UseFunction(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *assemble(command_t *p,cons_t *cons,hash_table_t *hash);
command_t *asm_DefunUseFunction(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash);
command_t *asm_DefunOp(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash);
command_t *asm_DefunIf(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash);

command_t *ListRun_New()
{
	command_t *command_t_ptr = (command_t *)malloc(sizeof(command_t));
	memset(command_t_ptr,0x00,sizeof(command_t));
	command_t_ptr->v = (value_t *)malloc(sizeof(value_t));
	return command_t_ptr;
}

void freeListRun(command_t *p)
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

void compile(cons_t *ast,command_t *root,hash_table_t *hash)
{
	cons_t *chain = ast;
	command_t *p = root;
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

command_t *assemble(command_t *p,cons_t *cons,hash_table_t *hash)
{
	switch(cons->type) {
	case TY_Cdr:
		setBoolean(p,"Nil");
		break;
	case TY_Op:
		p = asm_Op(p,cons,hash);
		break;
	case TY_EOL:
		p->command = C_End;
		break;
	case TY_Str:
		p = asm_UseFunction(p,cons,hash);
		break;
	case TY_Setq:
		p = asm_Setq(p,cons,hash);
		break;
	case TY_Defun:
		p = asm_Defun(p,cons,hash);
		break;
	case TY_If:
		p = asm_If(p,cons,hash);
		break;
	default:
		printf("some error occured in assemble().\n");
		break;
	}
	return p;
}

command_t *asm_Car(command_t *cmd,cons_t *cons,hash_table_t *hash)
{
	if(cons->car->type == TY_Str) {
		return asm_UseFunction(cmd,cons->car,hash);
	}else if(cons->car->type == TY_If) {
		return asm_If(cmd,cons->car,hash);
	}
	return asm_Op(cmd,cons->car,hash);
}

command_t *asm_DefunCar(command_t *cmd,cons_t *cons,hash_table_t *argument,hash_table_t *hash)
{
	if(cons->car->type == TY_Str) {
		return asm_DefunUseFunction(cmd,cons->car,argument,hash);
	}else if(cons->car->type == TY_If) {
		return asm_DefunIf(cmd,cons->car,argument,hash);
	}
	return asm_DefunOp(cmd,cons->car,argument,hash);
}

command_t *asm_Defun(command_t *cmd,cons_t *cons,hash_table_t *hash)
{
	command_t *func = ListRun_New();
	HashTable_insert_Function(hash,cons->cdr->string.s,cons->cdr->string.len,func);
	command_t *list = func;

	hash_table_t *argument = HashTable_init();

	cons_t *tmp = cons->cdr->cdr->car;
	int i = 1;
	while(tmp->type != TY_Cdr) {
		value_t *v = (value_t *)malloc(sizeof(value_t));
		v->num = i;
		HashTable_insert_Value(argument,tmp->string.s,tmp->string.len,v);
		tmp = tmp->cdr;
		++i;
	}
	list = asm_DefunCar(list,cons->cdr->cdr->cdr,argument,hash);
	list->command = C_End;
	list->iseq = tables[C_End];
	

	HashTable_free(argument);
	return cmd;
}

command_t *asm_If(command_t *cmd,cons_t *cons, hash_table_t *hash)
{
	command_t *list = cmd;
	//condition
	list = asm_Car(list,cons->cdr,hash);

	//tag jump
	list->command = C_TJump;
	list->iseq = tables[C_TJump];
	command_t *t_jump = ListRun_New();
	list->v->func = t_jump;
	list = ListRun_New();
	list = list->next;

	//if true
	t_jump = asm_Car(t_jump,cons->cdr->cdr,hash);

	//if false
	list = asm_Car(list,cons->cdr->cdr->cdr,hash);

	t_jump->command = C_Tag;
	t_jump->iseq = tables[C_Tag];
	t_jump->next = list;

	return list;
}

command_t *asm_Setq(command_t *cmd,cons_t *cons, hash_table_t *hash)
{
	cons_t *p = cons->cdr->cdr;
	command_t *list = cmd;

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
		printf("some error occonsred in asm_Setq().\n");
		break;
	}
	


	list->command = C_SetHash;
	list->v->type = Pointer;
	list->v->string.s = cons->cdr->string.s;
	list->v->string.len = cons->cdr->string.len;
	list->iseq = tables[C_SetHash];

	list->next = ListRun_New();
	list = list->next;
	return list;
}

command_t *asm_Op(command_t *cmd,cons_t *cons,hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	command_t *list = cmd;

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

	switch(cons->string.s[0]) {
	case '+':
		setOp(list,C_OpPlus,count);
		break;
	case '-':
		setOp(list,C_OpMinus,count);
		break;
	case '*':
		setOp(list,C_OpMul,count);
		break;
	case '/':
		setOp(list,C_OpDiv,count);
		break;
	case '<':
		setOp(list,C_OpLt,count);
		break;
	case '>':
		setOp(list,C_OpGt,count);
		break;
	default:
		printf("some error occonsred in asm_Op() 2. %s\n",cons->string.s);
		break;
	}
	list->next = ListRun_New();
	list = list->next;
	return list;
}

command_t *asm_UseFunction(command_t *cmd,cons_t *cons,hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	command_t *list = cmd;

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
			printf("some error occonsred in asm_UseFunction().\n");
			break;
		}
		++count;
		p = p->cdr;
	}

	if(strcmp(cons->string.s,"print") == 0) {
		list->command = C_Print;
		list->v->type = CALLFUNCTION;
		list->v->string.s = "print";
		list->v->string.len = strlen("print");
		list->iseq = tables[C_Print];
	}else{
		list->command = C_Call;
		list->v->type = CALLFUNCTION;
		list->v->num = count;
		list->v->string.s = cons->string.s;
		list->v->string.len = cons->string.len;
		list->v->func = HashTable_lookup_Function(hash, cons->string.s,cons->string.len);
		list->iseq = tables[C_Call];
	}

	list->next = ListRun_New();
	list = list->next;
	return list;
}

command_t *asm_DefunUseFunction(command_t *cmd,cons_t *cons,hash_table_t *argument, hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	command_t *list = cmd;

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

	if(strcmp(cons->string.s,"print") == 0) {
		list->command = C_Print;
		list->v->type = CALLFUNCTION;
		list->v->string.s = "print";
		list->v->string.len = strlen("print");
		list->iseq = tables[C_Print];
	}else{
		list->command = C_Call;
		list->v->type = CALLFUNCTION;
		list->v->num = count;
		list->v->string.s = cons->string.s;
		list->v->string.len = cons->string.len;
		list->v->func = HashTable_lookup_Function(hash, cons->string.s,cons->string.len);
		list->iseq = tables[C_Call];
	}

	list->next = ListRun_New();
	list = list->next;
	return list;
}

command_t *asm_DefunOp(command_t *cmd,cons_t *cons, hash_table_t *argument, hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	command_t *list = cmd;

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

	switch(cons->string.s[0]) {
	case '+':
		setOp(list,C_OpPlus,count);
		break;
	case '-':
		setOp(list,C_OpMinus,count);
		break;
	case '*':
		setOp(list,C_OpMul,count);
		break;
	case '/':
		setOp(list,C_OpDiv,count);
		break;
	case '<':
		setOp(list,C_OpLt,count);
		break;
	case '>':
		setOp(list,C_OpGt,count);
		break;
	default:
		printf("some error occonsred in asm_Op() 2. %s\n",cons->string.s);
		break;
	}
	list->next = ListRun_New();
	list = list->next;
	return list;
}

command_t *asm_DefunIf(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash)
{
	command_t *list = cmd;
	
	//condition
	list = asm_DefunCar(list,cons->cdr,argument,hash);

	//tag jump
	list->command = C_TJump;
	list->iseq = tables[C_TJump];
	command_t *t_jump = ListRun_New();
	list->v->func = t_jump;
	
	list->next = ListRun_New();
	list = list->next;

	//if true
	if(cons->cdr->cdr->type == TY_Car) {
		t_jump = asm_DefunCar(t_jump,cons->cdr->cdr,argument,hash);
	}else if(cons->cdr->cdr->type == TY_Value) {
		setValue(t_jump,cons->cdr->cdr->ivalue);
		t_jump->next = ListRun_New();
		t_jump = t_jump->next;
	}

	//if false
	if(cons->cdr->cdr->cdr->type == TY_Car) {
		list = asm_DefunCar(list,cons->cdr->cdr->cdr,argument,hash);
	}else if(cons->cdr->cdr->cdr->type == TY_Value) {
		setValue(list,cons->cdr->cdr->cdr->ivalue);
		list->next = ListRun_New();
		list = list->next;
	}
	//end_tag

	t_jump->command = C_Tag;
	t_jump->iseq = tables[C_Tag];
	t_jump->next = list;

	return list;
}
