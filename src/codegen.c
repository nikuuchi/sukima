#include "lisp.h"

#define setInt(c,d,arg) \
	do { \
		(c)->command = C_Put; \
		Int_init((c)->data[(arg)],(d)); \
		(c)->iseq = tables[C_Put]; \
	} while(0);

#define setDouble(c,f,arg) \
	do { \
		(c)->command = C_Put; \
		(c)->data[(arg)].d = (f);	   \
		(c)->iseq = tables[C_Put]; \
	} while(0);

#define setStr(c,d,e,arg) \
	do { \
		(c)->command = C_LoadValue; \
		String_Copy((c)->data[(arg)].string->s,(d),(e)); \
		(c)->data[(arg)].string->len = (e); \
		(c)->iseq = tables[C_LoadValue]; \
	} while(0);

#define setBoolean(c,d,arg) \
	do { \
		(c)->command = C_Put; \
		(c)->data[(arg)].string->s = (d); \
		(c)->iseq = tables[C_Put]; \
	} while(0);

#define setOp(c,d) \
	do { \
		(c)->command = (d); \
		(c)->iseq = tables[(d)]; \
	} while(0);

void **tables;
command_t *asm_LParen(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_Setq(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_If(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_Defun(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_Op(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *asm_CallFunction(command_t *cmd,cons_t *cons,hash_table_t *hash);
command_t *assemble(command_t *p,cons_t *cons,hash_table_t *hash);
command_t *asm_DefunCallFunction(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash);
command_t *asm_DefunOp(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash);
command_t *asm_DefunIf(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash);

void Command_free(command_t *p)
{
	if(p != NULL) {
		if(p->command != C_Nop) {
			if(p->command == C_TJump) {
				Command_free(p->data[0].pointer);
			}
			Command_free(p->next);
		}
	}
	free(p);
}

void compile(cons_t *ast,command_t *root,hash_table_t *hash)
{
	cons_t *chain = ast;
	command_t *p = root;
	value_t st[2];
	tables = vm_exec(root,st,0,hash,1);
	while(chain->cdr != NULL){
		switch(chain->type){
		case TY_LParen:
			p = assemble(p,chain->car,hash);
			break;
		case TY_Int:
			setInt(p,chain->ivalue,0);
			break;
		case TY_Double:
			setDouble(p,chain->fvalue,0);
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
	case TY_RParen:
		setBoolean(p,"Nil",0);
		break;
	case TY_Op:
		p = asm_Op(p,cons,hash);
		break;
	case TY_EOL:
		p->command = C_End;
		break;
	case TY_Str:
		p = asm_CallFunction(p,cons,hash);
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

command_t *asm_LParen(command_t *cmd,cons_t *cons,hash_table_t *hash)
{
	if(cons->car->type == TY_Str) {
		return asm_CallFunction(cmd,cons->car,hash);
	}else if(cons->car->type == TY_If) {
		return asm_If(cmd,cons->car,hash);
	}
	return asm_Op(cmd,cons->car,hash);
}

command_t *asm_DefunLParen(command_t *cmd,cons_t *cons,hash_table_t *argument,hash_table_t *hash)
{
	if(cons->car->type == TY_Str) {
		return asm_DefunCallFunction(cmd,cons->car,argument,hash);
	}else if(cons->car->type == TY_If) {
		return asm_DefunIf(cmd,cons->car,argument,hash);
	}
	return asm_DefunOp(cmd,cons->car,argument,hash);
}

command_t *asm_Defun(command_t *cmd,cons_t *cons,hash_table_t *hash)
{
	command_t *func = Command_New();
	HashTable_insert_Function(hash,cons->cdr->string.s,cons->cdr->string.len,func);
	command_t *list = func;

	hash_table_t *argument = HashTable_init();

	cons_t *tmp = cons->cdr->cdr->car;
	int i = 1;
	while(tmp->type != TY_RParen) {
		value_t *v = (value_t *)malloc(sizeof(value_t));
		v->i = i;
		HashTable_insert_Value(argument,tmp->string.s,tmp->string.len,v);
		tmp = tmp->cdr;
		++i;
	}
	list = asm_DefunLParen(list,cons->cdr->cdr->cdr,argument,hash);
	list->command = C_End;
	list->iseq = tables[C_End];

	HashTable_free(argument);
	return cmd;
}

command_t *asm_If(command_t *cmd,cons_t *cons, hash_table_t *hash)
{
	command_t *list = cmd;
	
	//condition
	list = asm_LParen(list,cons->cdr,hash);

	//tag jump
	list->command = C_TJump;
	list->iseq = tables[C_TJump];
	command_t *t_jump = Command_New();
	list->data[0].pointer = t_jump;
	
	list->next = Command_New();
	list = list->next;

	//if true
	if(cons->cdr->cdr->type == TY_LParen) {
		t_jump = asm_LParen(t_jump,cons->cdr->cdr,hash);
	}else if(cons->cdr->cdr->type == TY_Int) {
		setInt(t_jump,cons->cdr->cdr->ivalue,0);
		t_jump->next = Command_New();
		t_jump = t_jump->next;
	}else if(cons->cdr->cdr->type == TY_Double) {
		setDouble(t_jump,cons->cdr->cdr->fvalue,0);
		t_jump->next = Command_New();
		t_jump = t_jump->next;
	}

	//if false
	if(cons->cdr->cdr->cdr->type == TY_LParen) {
		list = asm_LParen(list,cons->cdr->cdr->cdr,hash);
	}else if(cons->cdr->cdr->cdr->type == TY_Int) {
		setInt(list,cons->cdr->cdr->cdr->ivalue,0);
		list->next = Command_New();
		list = list->next;
	}else if(cons->cdr->cdr->cdr->type == TY_Double) {
		setInt(list,cons->cdr->cdr->cdr->fvalue,0);
		list->next = Command_New();
		list = list->next;
	}
	//end_tag

	t_jump->command = C_Nop;
	t_jump->iseq = tables[C_Nop];
	t_jump->next = list;

	return list;
}

command_t *asm_Setq(command_t *cmd,cons_t *cons, hash_table_t *hash)
{
	cons_t *p = cons->cdr->cdr;
	command_t *list = cmd;

	switch(p->type) {
	case TY_LParen:
		list = asm_LParen(list,p,hash);
		break;
	case TY_Int:
		setInt(list,p->ivalue,0);
		list->next = Command_New();
		list = list->next;
		break;
	case TY_Double:
		setDouble(list,p->fvalue,0);
		list->next = Command_New();
		list = list->next;
		break;
	default:
		printf("some error occonsred in asm_Setq().\n");
		break;
	}
	


	list->command = C_SetHash;
	String_Copy(list->data[0].string->s,cons->cdr->string.s,cons->cdr->string.len);
	list->data[0].string->len = cons->cdr->string.len;
	list->iseq = tables[C_SetHash];

	list->next = Command_New();
	list = list->next;
	return list;
}

command_t *asm_Op(command_t *cmd,cons_t *cons,hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	command_t *list = cmd;

	while(p->type != TY_RParen) {
		switch(p->type) {
		case TY_LParen:
			list = asm_LParen(list,p,hash);
			break;
		case TY_Int:
			setInt(list,p->ivalue,0);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue,0);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Str:
			setStr(list,p->string.s,p->string.len,0);
			list->next = Command_New();
			list = list->next;
			break;
		default:
			printf("some error occured in asm_Op() 1.\n");
			break;
		}
		if(count > 0) {
			switch(cons->string.s[0]) {
			case '+':
				setOp(list,C_OpPlus);
				break;
			case '-':
				setOp(list,C_OpMinus);
				break;
			case '*':
				setOp(list,C_OpMul);
				break;
			case '/':
				setOp(list,C_OpDiv);
				break;
			case '<':
				setOp(list,C_OpLt);
				break;
			case '>':
				setOp(list,C_OpGt);
				break;
			default:
				printf("some error occonsred in asm_Op() 2. %s\n",cons->string.s);
				break;
			}
			list->next = Command_New();
			list = list->next;
		}
		++count;
		p = p->cdr;
	}

	return list;
}

command_t *asm_CallFunction(command_t *cmd,cons_t *cons,hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	command_t *list = cmd;

	while(p->type != TY_RParen) {
		switch(p->type) {
		case TY_LParen:
			list = asm_LParen(list,p,hash);
			break;
		case TY_Int:
			setInt(list,p->ivalue,0);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue,0);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Str:
			setStr(list,p->string.s,p->string.len,0);
			list->next = Command_New();
			list = list->next;
			break;
		default:
			printf("some error occonsred in asm_CallFunction().\n");
			break;
		}
		++count;
		p = p->cdr;
	}

	if(strcmp(cons->string.s,"print") == 0) {
		list->command = C_Print;
		list->iseq = tables[C_Print];
	}else{
		list->command = C_Call;
		list->data[1].i = count;
		list->data[0].pointer = HashTable_lookup_Function(hash, cons->string.s,cons->string.len);
		list->iseq = tables[C_Call];
	}

	list->next = Command_New();
	list = list->next;
	return list;
}

command_t *asm_DefunCallFunction(command_t *cmd,cons_t *cons,hash_table_t *argument, hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	command_t *list = cmd;

	while(p->type != TY_RParen) {
		switch(p->type) {
		case TY_LParen:
			list = asm_DefunLParen(list,p,argument,hash);
			break;
		case TY_Int:
			setInt(list,p->ivalue,0);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue,0);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Str: {
			value_t *v = HashTable_lookup_Value(argument,p->string.s,p->string.len);
			list->command = C_Args;
			list->data[0] = *v;
			list->iseq = tables[C_Args];
			list->next = Command_New();
			list = list->next;
			break;
		}
		default:
			printf("some error occured in asm_CallFunction().\n");
			break;
		}
		++count;
		p = p->cdr;
	}

	if(strcmp(cons->string.s,"print") == 0) {
		list->command = C_Print;
		list->iseq = tables[C_Print];
	}else{
		list->command = C_Call;
		list->data[1].i = count;
		list->data[0].pointer = HashTable_lookup_Function(hash, cons->string.s,cons->string.len);
		list->iseq = tables[C_Call];
	}

	list->next = Command_New();
	list = list->next;
	return list;
}

command_t *asm_DefunOp(command_t *cmd,cons_t *cons, hash_table_t *argument, hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	command_t *list = cmd;

	while(p->type != TY_RParen) {
		switch(p->type) {
		case TY_LParen:
			list = asm_DefunLParen(list,p,argument,hash);
			break;
		case TY_Int:
			setInt(list,p->ivalue,0);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue,0);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Str:{
			value_t *v = HashTable_lookup_Value(argument,p->string.s,p->string.len);
			list->command = C_Args;
			list->data[0] = *v;
			list->iseq = tables[C_Args];
			list->next = Command_New();
			list = list->next;
			break;
		}
		default:
			printf("some error occured in asm_Op() 1.\n");
			break;
		}
		if(count > 0) {
			switch(cons->string.s[0]) {
			case '+':
				setOp(list,C_OpPlus);
				break;
			case '-':
				setOp(list,C_OpMinus);
				break;
			case '*':
				setOp(list,C_OpMul);
				break;
			case '/':
				setOp(list,C_OpDiv);
				break;
			case '<':
				setOp(list,C_OpLt);
				break;
			case '>':
				setOp(list,C_OpGt);
				break;
			default:
				printf("some error occonsred in asm_Op() 2. %s\n",cons->string.s);
				break;
			}
			list->next = Command_New();
			list = list->next;
		}
		++count;
		p = p->cdr;
	}
	return list;
}

command_t *asm_DefunIf(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash)
{
	command_t *list = cmd;
	
	//condition
	list = asm_DefunLParen(list,cons->cdr,argument,hash);

	//tag jump
	list->command = C_TJump;
	list->iseq = tables[C_TJump];
	command_t *t_jump = Command_New();
	list->data[0].pointer = t_jump;
	
	list->next = Command_New();
	list = list->next;

	//if true
	if(cons->cdr->cdr->type == TY_LParen) {
		t_jump = asm_DefunLParen(t_jump,cons->cdr->cdr,argument,hash);
	}else if(cons->cdr->cdr->type == TY_Int) {
		setInt(t_jump,cons->cdr->cdr->ivalue,0);
		t_jump->next = Command_New();
		t_jump = t_jump->next;
	}else if(cons->cdr->cdr->type == TY_Double) {
		setDouble(t_jump,cons->cdr->cdr->fvalue,0);
		t_jump->next = Command_New();
		t_jump = t_jump->next;
	}

	//if false
	if(cons->cdr->cdr->cdr->type == TY_LParen) {
		list = asm_DefunLParen(list,cons->cdr->cdr->cdr,argument,hash);
	}else if(cons->cdr->cdr->cdr->type == TY_Int) {
		setInt(list,cons->cdr->cdr->cdr->ivalue,0);
		list->next = Command_New();
		list = list->next;
	}else if(cons->cdr->cdr->cdr->type == TY_Double) {
		setDouble(list,cons->cdr->cdr->cdr->fvalue,0);
		list->next = Command_New();
		list = list->next;
	}
	//end_tag

	t_jump->command = C_Nop;
	t_jump->iseq = tables[C_Nop];
	t_jump->next = list;

	return list;
}
