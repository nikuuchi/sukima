#include "lisp.h"

static bytecode_t *asm_LParen(bytecode_t *opcode,cons_t *cons,hash_table_t *hash);
static bytecode_t *asm_Setq(bytecode_t *opcode,cons_t *cons,hash_table_t *hash);
static bytecode_t *asm_If(bytecode_t *opcode,cons_t *cons,hash_table_t *hash);
static bytecode_t *asm_Defun(bytecode_t *opcode,cons_t *cons,hash_table_t *hash);
static bytecode_t *asm_Op(bytecode_t *opcode,cons_t *cons,hash_table_t *hash);
static bytecode_t *asm_CallFunction(bytecode_t *opcode,cons_t *cons,hash_table_t *hash);
static bytecode_t *assemble(bytecode_t *p,cons_t *cons,hash_table_t *hash);
static bytecode_t *asm_DefunCallFunction(bytecode_t *opcode,cons_t *cons, hash_table_t *argument,hash_table_t *hash);
static bytecode_t *asm_DefunOp(bytecode_t *opcode,cons_t *cons, hash_table_t *argument,hash_table_t *hash);
static bytecode_t *asm_DefunIf(bytecode_t *opcode,cons_t *cons, hash_table_t *argument,hash_table_t *hash);

const void **tables;

static void setInt(bytecode_t *c,int d)
{
	c->code = C_Put;
	c->data = Int_init(d);
	c->iseq = tables[C_Put];
}

static void setDouble(bytecode_t *c,double d)
{
	c->code = C_Put;
	c->data.d = d;
	c->iseq = tables[C_Put];
}

static void setCStr(bytecode_t *c,char *d,int len)
{
	c->code = C_Put;
	String_Copy(String_Ptr(c->data)->s,d,len);
	String_Ptr(c->data)->len = len;
	c->iseq = tables[C_Put];
}

static void setStr(bytecode_t *c,char *d,int len)
{
	c->code = C_LoadValue;
	String_Copy(String_Ptr(c->data)->s,d,len);
	String_Ptr(c->data)->len = len;
	c->iseq = tables[C_LoadValue];
}

static void setOp(bytecode_t *c,OpCode d)
{
	c->code = d;
	c->iseq = tables[d];
}
static void setCOpi(bytecode_t *c,OpCode d,int i)
{
	if(d == C_OpCPlus && i == 1){
		c->code = C_Inc;
		c->iseq = tables[C_Inc];
	}else if(d == C_OpCMinus && i == 1) {
		c->code = C_Dec;
		c->iseq = tables[C_Dec];
	}else{
		c->code = d;
		c->data = Int_init(i);
		c->iseq = tables[d];
	}
}
static void setCOpd(bytecode_t *c,OpCode e,double d)
{
	c->code = e;
	c->data.d = d;
	c->iseq = tables[e];
}

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

void Bytecode_free(bytecode_t *p)
{
	if(p->code != C_Nop) {
		if(p->code == C_TJump) {
			Bytecode_free(p->data.o);
		}else if(p->code == C_SetHash || p->code == C_LoadValue) {
			free(String_Ptr(p->data)->s);
			free(String_Ptr(p->data));
		}
	}
	if(p->next != NULL)
		Bytecode_free(p->next);
	free(p);
}

void compile(cons_t *ast,bytecode_t *root,hash_table_t *hash)
{
	cons_t *chain = ast;
	bytecode_t *p = root;
	value_t st[1];
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
	p->code = C_Ret;
	p->iseq = tables[C_Ret];
}

static bytecode_t *assemble(bytecode_t *p,cons_t *cons,hash_table_t *hash)
{
	switch(cons->type) {
	case TY_RParen:
		break;
	case TY_Op:
		p = asm_Op(p,cons,hash);
		break;
	case TY_EOL:
		p->code = C_Ret;
		p->iseq = tables[C_Ret];
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

static bytecode_t *asm_LParen(bytecode_t *opcode,cons_t *cons,hash_table_t *hash)
{
	if(cons->car->type == TY_Str) {
		return asm_CallFunction(opcode,cons->car,hash);
	}else if(cons->car->type == TY_If) {
		return asm_If(opcode,cons->car,hash);
	}
	return asm_Op(opcode,cons->car,hash);
}

static bytecode_t *asm_DefunLParen(bytecode_t *opcode,cons_t *cons,hash_table_t *argument,hash_table_t *hash)
{
	if(cons->car->type == TY_Str) {
		return asm_DefunCallFunction(opcode,cons->car,argument,hash);
	}else if(cons->car->type == TY_If) {
		return asm_DefunIf(opcode,cons->car,argument,hash);
	}
	return asm_DefunOp(opcode,cons->car,argument,hash);
}

static bytecode_t *asm_Defun(bytecode_t *opcode,cons_t *cons,hash_table_t *hash)
{
	bytecode_t *func = Bytecode_New();
	HashTable_insert_Function(hash,cons->cdr->string.s,cons->cdr->string.len,func);
	bytecode_t *list = func;

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
	list->code = C_Ret;
	list->iseq = tables[C_Ret];

	HashTable_free(argument);
	return opcode;
}

static bytecode_t *asm_If(bytecode_t *opcode,cons_t *cons, hash_table_t *hash)
{
	bytecode_t *list = opcode;
	
	//condition
	list = asm_LParen(list,cons->cdr,hash);

	//tag jump
	list->code = C_TJump;
	list->iseq = tables[C_TJump];
	bytecode_t *t_jump = Bytecode_New();
	list->data.o = t_jump;
	
	list->next = Bytecode_New();
	list = list->next;

	//if true
	if(cons->cdr->cdr->type == TY_LParen) {
		t_jump = asm_LParen(t_jump,cons->cdr->cdr,hash);
	}else if(cons->cdr->cdr->type == TY_Int) {
		setInt(t_jump,cons->cdr->cdr->ivalue);
		t_jump->next = Bytecode_New();
		t_jump = t_jump->next;
	}else if(cons->cdr->cdr->type == TY_Double) {
		setDouble(t_jump,cons->cdr->cdr->fvalue);
		t_jump->next = Bytecode_New();
		t_jump = t_jump->next;
	}

	//if false
	if(cons->cdr->cdr->cdr->type == TY_LParen) {
		list = asm_LParen(list,cons->cdr->cdr->cdr,hash);
	}else if(cons->cdr->cdr->cdr->type == TY_Int) {
		setInt(list,cons->cdr->cdr->cdr->ivalue);
		list->next = Bytecode_New();
		list = list->next;
	}else if(cons->cdr->cdr->cdr->type == TY_Double) {
		setInt(list,cons->cdr->cdr->cdr->fvalue);
		list->next = Bytecode_New();
		list = list->next;
	}
	//end_tag

	t_jump->code = C_Nop;
	t_jump->iseq = tables[C_Nop];
	t_jump->next = list;

	return list;
}

static bytecode_t *asm_Setq(bytecode_t *opcode,cons_t *cons, hash_table_t *hash)
{
	cons_t *p = cons->cdr->cdr;
	bytecode_t *list = opcode;

	switch(p->type) {
	case TY_LParen:
		list = asm_LParen(list,p,hash);
		break;
	case TY_Int:
		setInt(list,p->ivalue);
		list->next = Bytecode_New();
		list = list->next;
		break;
	case TY_Double:
		setDouble(list,p->fvalue);
		list->next = Bytecode_New();
		list = list->next;
		break;
	case TY_CStr:
		list->data = String_init();
		setCStr(list,p->string.s,p->string.len);
		list->next = Bytecode_New();
		list = list->next;
		break;
	default:
		printf("some error occonsred in asm_Setq().\n");
		break;
	}
	


	list->code = C_SetHash;
	list->data = String_init();
	String_Copy(String_Ptr(list->data)->s,cons->cdr->string.s,cons->cdr->string.len);
	String_Ptr(list->data)->len = cons->cdr->string.len;
	list->iseq = tables[C_SetHash];

	list->next = Bytecode_New();
	list = list->next;
	return list;
}

static bytecode_t *asm_Op(bytecode_t *opcode,cons_t *cons,hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	bytecode_t *list = opcode;

	OpCode opc = C_OpCPlus;
	OpCode op  = C_OpPlus;
	switch(cons->string.s[0]) {
	case '+':
		opc = C_OpCPlus;
		op  = C_OpPlus;
		break;
	case '-':
		opc = C_OpCMinus;
		op  = C_OpMinus;
		break;
	case '*':
		opc = C_OpCMul;
		op  = C_OpMul;
		break;
	case '/':
		opc = C_OpCDiv;
		op  = C_OpDiv;
		break;
	case '<':
		opc = C_OpCLt;
		op  = C_OpLt;
		break;
	case '>':
		opc = C_OpCGt;
		op  = C_OpGt;
		break;
	case '=':
	case 'e':
		opc = C_OpCEq;
		op  = C_OpEq;
		break;
	case 'm':
		opc = C_OpCMod;
		op  = C_OpMod;
		break;
	default:
		printf("some error occonsred in asm_Op() 2. %s\n",cons->string.s);
		break;
	}

	switch(p->type) {
	case TY_LParen:
		list = asm_LParen(list,p,hash);
		break;
	case TY_Int:
		setInt(list,p->ivalue);
		list->next = Bytecode_New();
		list = list->next;
		break;
	case TY_Double:
		setDouble(list,p->fvalue);
		list->next = Bytecode_New();
		list = list->next;
		break;
	case TY_Str:
		setStr(list,p->string.s,p->string.len);
		list->next = Bytecode_New();
		list = list->next;
		break;
	default:
		printf("some error occured in asm_Op() 1.\n");
		break;
	}
	p = p->cdr;
	while(p->type != TY_RParen) {
		switch(p->type) {
		case TY_LParen:
			list = asm_LParen(list,p,hash);
			setOp(list,op);
			break;
		case TY_Int:
			setCOpi(list,opc,p->ivalue);
			break;
		case TY_Double:
			setCOpd(list,opc,p->fvalue);
			break;
		case TY_Str:
			setStr(list,p->string.s,p->string.len);
			list->next = Bytecode_New();
			list = list->next;
			setOp(list,op);
			break;
		default:
			printf("some error occured in asm_Op() 1.\n");
			break;
		}
		list->next = Bytecode_New();
		list = list->next;
		++count;
		p = p->cdr;
	}

	return list;
}

static bytecode_t *asm_CallFunction(bytecode_t *opcode,cons_t *cons,hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	bytecode_t *list = opcode;

	while(p->type != TY_RParen) {
		switch(p->type) {
		case TY_LParen:
			list = asm_LParen(list,p,hash);
			break;
		case TY_Int:
			setInt(list,p->ivalue);
			list->next = Bytecode_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue);
			list->next = Bytecode_New();
			list = list->next;
			break;
		case TY_Str:
			list->data = String_init();
			setStr(list,p->string.s,p->string.len);
			list->next = Bytecode_New();
			list = list->next;
			break;
		case TY_CStr:
			list->data = String_init();
			setCStr(list,p->string.s,p->string.len);
			list->next = Bytecode_New();
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
		list->code = C_Print;
		list->iseq = tables[C_Print];
	}else{
		list->code = C_Call;
		list->data.o = HashTable_lookup_Function(hash, cons->string.s,cons->string.len);
		list->iseq = tables[C_Call];
		list->next = Bytecode_New();
		list = list->next;
		list->code = C_Print;
		list->iseq = tables[C_Print];
	}

	list->next = Bytecode_New();
	list = list->next;
	return list;
}

static bytecode_t *asm_DefunCallFunction(bytecode_t *opcode,cons_t *cons,hash_table_t *argument, hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	bytecode_t *list = opcode;

	while(p->type != TY_RParen) {
		switch(p->type) {
		case TY_LParen:
			list = asm_DefunLParen(list,p,argument,hash);
			break;
		case TY_Int:
			setInt(list,p->ivalue);
			list->next = Bytecode_New();
			list = list->next;
			break;
		case TY_Double:
			setDouble(list,p->fvalue);
			list->next = Bytecode_New();
			list = list->next;
			break;
		case TY_CStr:
			list->data = String_init();
			setCStr(list,p->string.s,p->string.len);
			list->next = Bytecode_New();
			list = list->next;
			break;
		case TY_Str: {
			value_t *v = HashTable_lookup_Value(argument,p->string.s,p->string.len);
			list->code = C_Args;
			list->data = *v;
			list->iseq = tables[C_Args];
			list->next = Bytecode_New();
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
		list->code = C_Print;
		list->iseq = tables[C_Print];
	}else{
		list->code = C_Call;
		list->data.o = HashTable_lookup_Function(hash, cons->string.s,cons->string.len);
		list->iseq = tables[C_Call];
	}

	list->next = Bytecode_New();
	list = list->next;
	return list;
}

static bytecode_t *asm_DefunOp(bytecode_t *opcode, cons_t *cons, hash_table_t *argument, hash_table_t *hash)
{
	int count = 0;
	cons_t *p = cons->cdr;
	bytecode_t *list = opcode;

	OpCode opc = C_OpCPlus;
	OpCode op  = C_OpPlus;
	switch(cons->string.s[0]) {
	case '+':
		opc = C_OpCPlus;
		op  = C_OpPlus;
		break;
	case '-':
		opc = C_OpCMinus;
		op  = C_OpMinus;
		break;
	case '*':
		opc = C_OpCMul;
		op  = C_OpMul;
		break;
	case '/':
		opc = C_OpCDiv;
		op  = C_OpDiv;
		break;
	case '<':
		opc = C_OpCLt;
		op  = C_OpLt;
		break;
	case '>':
		opc = C_OpCGt;
		op  = C_OpGt;
		break;
	case '=':
	case 'e':
		opc = C_OpCEq;
		op  = C_OpEq;
		break;
	case 'm':
		opc = C_OpCMod;
		op  = C_OpMod;
		break;
	default:
		printf("some error occonsred in asm_Op() 2. %s\n",cons->string.s);
		break;
	}
	switch(p->type) {
	case TY_LParen:
		list = asm_DefunLParen(list,p,argument,hash);
		break;
	case TY_Int:
		setInt(list,p->ivalue);
		list->next = Bytecode_New();
		list = list->next;
		break;
	case TY_Double:
		setDouble(list,p->fvalue);
		list->next = Bytecode_New();
		list = list->next;
		break;
	case TY_Str:{
		value_t *v = HashTable_lookup_Value(argument,p->string.s,p->string.len);
		list->code = C_Args;
		list->data = *v;
		list->iseq = tables[C_Args];
		list->next = Bytecode_New();
		list = list->next;
		break;
	}
	default:
		printf("some error occured in asm_Op() 1.\n");
		break;
	}
	p = p->cdr;
	while(p->type != TY_RParen) {
		switch(p->type) {
		case TY_LParen:
			list = asm_DefunLParen(list,p,argument,hash);
			setOp(list,op);
			break;
		case TY_Int:
			setCOpi(list,opc,p->ivalue);
			break;
		case TY_Double:
			setCOpd(list,opc,p->fvalue);
			break;
		case TY_Str:{
			value_t *v = HashTable_lookup_Value(argument,p->string.s,p->string.len);
			list->code = C_Args;
			list->data = *v;
			list->iseq = tables[C_Args];
			list->next = Bytecode_New();
			list = list->next;
			setOp(list,op);
			break;
		default:
			printf("some error occured in asm_Op() 1.\n");
			break;
		}
		}
		list->next = Bytecode_New();
		list = list->next;
		++count;
		p = p->cdr;
	}
	return list;
}

static bytecode_t *asm_DefunIf(bytecode_t *opcode,cons_t *cons, hash_table_t *argument,hash_table_t *hash)
{
	bytecode_t *list = opcode;
	
	//condition
	list = asm_DefunLParen(list,cons->cdr,argument,hash);

	//tag jump
	list->code = C_TJump;
	list->iseq = tables[C_TJump];
	bytecode_t *t_jump = Bytecode_New();
	list->data.o = t_jump;
	
	list->next = Bytecode_New();
	list = list->next;

	//if true
	if(cons->cdr->cdr->type == TY_LParen) {
		t_jump = asm_DefunLParen(t_jump,cons->cdr->cdr,argument,hash);
	}else if(cons->cdr->cdr->type == TY_Int) {
		setInt(t_jump,cons->cdr->cdr->ivalue);
		t_jump->next = Bytecode_New();
		t_jump = t_jump->next;
	}else if(cons->cdr->cdr->type == TY_Double) {
		setDouble(t_jump,cons->cdr->cdr->fvalue);
		t_jump->next = Bytecode_New();
		t_jump = t_jump->next;
	}else if(cons->cdr->cdr->type == TY_CStr) {
		t_jump->data = String_init();
		setCStr(t_jump,cons->cdr->cdr->string.s,cons->cdr->cdr->string.len);
		t_jump->next = Bytecode_New();
		t_jump = t_jump->next;
	}

	//if false
	if(cons->cdr->cdr->cdr->type == TY_LParen) {
		list = asm_DefunLParen(list,cons->cdr->cdr->cdr,argument,hash);
	}else if(cons->cdr->cdr->cdr->type == TY_Int) {
		setInt(list,cons->cdr->cdr->cdr->ivalue);
		list->next = Bytecode_New();
		list = list->next;
	}else if(cons->cdr->cdr->cdr->type == TY_Double) {
		setDouble(list,cons->cdr->cdr->cdr->fvalue);
		list->next = Bytecode_New();
		list = list->next;
	}else if(cons->cdr->cdr->cdr->type == TY_CStr) {
		list->data = String_init();
		setCStr(list,cons->cdr->cdr->cdr->string.s,cons->cdr->cdr->cdr->string.len);
		list->next = Bytecode_New();
		list = list->next;
	}
	//end_tag

	t_jump->code = C_Nop;
	t_jump->iseq = tables[C_Nop];
	t_jump->next = list;

	return list;
}
