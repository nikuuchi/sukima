#include "lisp.h"

static void setInt(command_t *c,int d)
{
	c->command = C_Put;
	c->data = Int_init(d);
	c->iseq = tables[C_Put];
}

static void setDouble(command_t *c,double d)
{
	c->command = C_Put;
	c->data.d = d;
	c->iseq = tables[C_Put];
}

static void setCStr(command_t *c,char *d,int len)
{
	c->command = C_Put;
	String_Copy(String_Ptr(c->data)->s,d,len);
	String_Ptr(c->data)->len = len;
	c->iseq = tables[C_Put];
}

static void setStr(command_t *c,char *d,int len)
{
	c->command = C_LoadValue;
	String_Copy(String_Ptr(c->data)->s,d,len);
	String_Ptr(c->data)->len = len;
	c->iseq = tables[C_LoadValue];
}

static void setOp(command_t *c,Command d)
{
	c->command = d;
	c->iseq = tables[d];
}

const void **tables;
static command_t *asm_LParen(command_t *cmd,cons_t *cons,hash_table_t *hash);
static command_t *asm_Setq(command_t *cmd,cons_t *cons,hash_table_t *hash);
static command_t *asm_If(command_t *cmd,cons_t *cons,hash_table_t *hash);
static command_t *asm_Defun(command_t *cmd,cons_t *cons,hash_table_t *hash);
static command_t *asm_Op(command_t *cmd,cons_t *cons,hash_table_t *hash);
static command_t *asm_CallFunction(command_t *cmd,cons_t *cons,hash_table_t *hash);
static command_t *assemble(command_t *p,cons_t *cons,hash_table_t *hash);
static command_t *asm_DefunCallFunction(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash);
static command_t *asm_DefunOp(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash);
static command_t *asm_DefunIf(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash);

static value_t String_init(){
	struct string *str = (struct string *)calloc(1,sizeof(struct string));
	value_t t;
	t.string = str;
	t.bytes |= NaN | StringTag;
	return t;
}

static value_t List_init(){
	List_t *l = (List_t *)calloc(1,sizeof(List_t));
	value_t t;
	t.o = l;
	t.bytes |= NaN | ListTag;
	return t;
}

void Command_free(command_t *p)
{
	if(p != NULL) {
		if(p->command != C_Nop) {
			if(p->command == C_TJump) {
				Command_free(p->data.o);
			}else if(p->command == C_SetHash || p->command == C_LoadValue) {
				free(String_Ptr(p->data)->s);
				free(String_Ptr(p->data));
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
			setInt(p,chain->ivalue);
			break;
		case TY_Double:
			setDouble(p,chain->fvalue);
			break;
		case TY_CStr:
			p->data = String_init();
			setCStr(p,chain->string.s,chain->string.len);
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

static command_t *assemble(command_t *p,cons_t *cons,hash_table_t *hash)
{
	switch(cons->type) {
	case TY_RParen:
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

static command_t *asm_LParen(command_t *cmd,cons_t *cons,hash_table_t *hash)
{
	if(cons->car->type == TY_Str) {
		return asm_CallFunction(cmd,cons->car,hash);
	}else if(cons->car->type == TY_If) {
		return asm_If(cmd,cons->car,hash);
	}
	return asm_Op(cmd,cons->car,hash);
}

static command_t *asm_DefunLParen(command_t *cmd,cons_t *cons,hash_table_t *argument,hash_table_t *hash)
{
	if(cons->car->type == TY_Str) {
		return asm_DefunCallFunction(cmd,cons->car,argument,hash);
	}else if(cons->car->type == TY_If) {
		return asm_DefunIf(cmd,cons->car,argument,hash);
	}
	return asm_DefunOp(cmd,cons->car,argument,hash);
}

static command_t *asm_Defun(command_t *cmd,cons_t *cons,hash_table_t *hash)
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

static command_t *asm_If(command_t *cmd,cons_t *cons, hash_table_t *hash)
{
	command_t *list = cmd;
	
	//condition
	list = asm_LParen(list,cons->cdr,hash);

	//tag jump
	list->command = C_TJump;
	list->iseq = tables[C_TJump];
	command_t *t_jump = Command_New();
	list->data.o = t_jump;
	
	list->next = Command_New();
	list = list->next;

	//if true
	if(cons->cdr->cdr->type == TY_LParen) {
		t_jump = asm_LParen(t_jump,cons->cdr->cdr,hash);
	}else if(cons->cdr->cdr->type == TY_Int) {
		setInt(t_jump,cons->cdr->cdr->ivalue);
		t_jump->next = Command_New();
		t_jump = t_jump->next;
	}else if(cons->cdr->cdr->type == TY_Double) {
		setDouble(t_jump,cons->cdr->cdr->fvalue);
		t_jump->next = Command_New();
		t_jump = t_jump->next;
	}

	//if false
	if(cons->cdr->cdr->cdr->type == TY_LParen) {
		list = asm_LParen(list,cons->cdr->cdr->cdr,hash);
	}else if(cons->cdr->cdr->cdr->type == TY_Int) {
		setInt(list,cons->cdr->cdr->cdr->ivalue);
		list->next = Command_New();
		list = list->next;
	}else if(cons->cdr->cdr->cdr->type == TY_Double) {
		setInt(list,cons->cdr->cdr->cdr->fvalue);
		list->next = Command_New();
		list = list->next;
	}
	//end_tag

	t_jump->command = C_Nop;
	t_jump->iseq = tables[C_Nop];
	t_jump->next = list;

	return list;
}

static command_t *asm_Setq(command_t *cmd,cons_t *cons, hash_table_t *hash)
{
	cons_t *p = cons->cdr->cdr;
	command_t *list = cmd;

	switch(p->type) {
	case TY_LParen:
		list = asm_LParen(list,p,hash);
		break;
	case TY_Int:
		setInt(list,p->ivalue);
		list->next = Command_New();
		list = list->next;
		break;
	case TY_Double:
		setDouble(list,p->fvalue);
		list->next = Command_New();
		list = list->next;
		break;
	case TY_CStr:
		list->data = String_init();
		setCStr(list,p->string.s,p->string.len);
		list->next = Command_New();
		list = list->next;
		break;
	default:
		printf("some error occonsred in asm_Setq().\n");
		break;
	}
	


	list->command = C_SetHash;
	list->data = String_init();
	String_Copy(String_Ptr(list->data)->s,cons->cdr->string.s,cons->cdr->string.len);
	String_Ptr(list->data)->len = cons->cdr->string.len;
	list->iseq = tables[C_SetHash];

	list->next = Command_New();
	list = list->next;
	return list;
}

static command_t *asm_Op(command_t *cmd,cons_t *cons,hash_table_t *hash)
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
			setInt(list,p->ivalue);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Str:
			setStr(list,p->string.s,p->string.len);
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
			case '=':
			case 'e':
				setOp(list,C_OpEq);
				break;
			case 'm':
				setOp(list,C_OpMod);
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

static command_t *asm_CallFunction(command_t *cmd,cons_t *cons,hash_table_t *hash)
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
			setInt(list,p->ivalue);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Str:
			list->data = String_init();
			setStr(list,p->string.s,p->string.len);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_CStr:
			list->data = String_init();
			setCStr(list,p->string.s,p->string.len);
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
		list->data.o = HashTable_lookup_Function(hash, cons->string.s,cons->string.len);
		list->iseq = tables[C_Call];
	}

	list->next = Command_New();
	list = list->next;
	return list;
}

static command_t *asm_DefunCallFunction(command_t *cmd,cons_t *cons,hash_table_t *argument, hash_table_t *hash)
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
			setInt(list,p->ivalue);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_CStr:
			list->data = String_init();
			setCStr(list,p->string.s,p->string.len);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Str: {
			value_t *v = HashTable_lookup_Value(argument,p->string.s,p->string.len);
			list->command = C_Args;
			list->data = *v;
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
		list->data.o = HashTable_lookup_Function(hash, cons->string.s,cons->string.len);
		list->iseq = tables[C_Call];
	}

	list->next = Command_New();
	list = list->next;
	return list;
}

static command_t *asm_DefunOp(command_t *cmd, cons_t *cons, hash_table_t *argument, hash_table_t *hash)
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
			setInt(list,p->ivalue);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue);
			list->next = Command_New();
			list = list->next;
			break;
		case TY_Str:{
			value_t *v = HashTable_lookup_Value(argument,p->string.s,p->string.len);
			list->command = C_Args;
			list->data = *v;
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
			case '=':
			case 'e':
				setOp(list,C_OpEq);
				break;
			case 'm':
				setOp(list,C_OpMod);
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

static command_t *asm_DefunIf(command_t *cmd,cons_t *cons, hash_table_t *argument,hash_table_t *hash)
{
	command_t *list = cmd;
	
	//condition
	list = asm_DefunLParen(list,cons->cdr,argument,hash);

	//tag jump
	list->command = C_TJump;
	list->iseq = tables[C_TJump];
	command_t *t_jump = Command_New();
	list->data.o = t_jump;
	
	list->next = Command_New();
	list = list->next;

	//if true
	if(cons->cdr->cdr->type == TY_LParen) {
		t_jump = asm_DefunLParen(t_jump,cons->cdr->cdr,argument,hash);
	}else if(cons->cdr->cdr->type == TY_Int) {
		setInt(t_jump,cons->cdr->cdr->ivalue);
		t_jump->next = Command_New();
		t_jump = t_jump->next;
	}else if(cons->cdr->cdr->type == TY_Double) {
		setDouble(t_jump,cons->cdr->cdr->fvalue);
		t_jump->next = Command_New();
		t_jump = t_jump->next;
	}else if(cons->cdr->cdr->type == TY_CStr) {
		t_jump->data = String_init();
		setCStr(t_jump,cons->cdr->cdr->string.s,cons->cdr->cdr->string.len);
		t_jump->next = Command_New();
		t_jump = t_jump->next;
	}

	//if false
	if(cons->cdr->cdr->cdr->type == TY_LParen) {
		list = asm_DefunLParen(list,cons->cdr->cdr->cdr,argument,hash);
	}else if(cons->cdr->cdr->cdr->type == TY_Int) {
		setInt(list,cons->cdr->cdr->cdr->ivalue);
		list->next = Command_New();
		list = list->next;
	}else if(cons->cdr->cdr->cdr->type == TY_Double) {
		setDouble(list,cons->cdr->cdr->cdr->fvalue);
		list->next = Command_New();
		list = list->next;
	}else if(cons->cdr->cdr->cdr->type == TY_CStr) {
		list->data = String_init();
		setCStr(list,cons->cdr->cdr->cdr->string.s,cons->cdr->cdr->cdr->string.len);
		list->next = Command_New();
		list = list->next;
	}
	//end_tag

	t_jump->command = C_Nop;
	t_jump->iseq = tables[C_Nop];
	t_jump->next = list;

	return list;
}
