#include "lisp.h"

#define push(a) st[esp++] = (a)
#define pop() (st[--esp])

static int Type_Check(value_t t){
	return (((t.bytes & NaN) == NaN) * ((t.bytes & TYPE) >> 48 ));
}

static void PrintInt(value_t v)
{
	printf("%d\n",v.i);
}

static void PrintDouble(value_t v)
{
	printf("%f\n",v.d);
}

static void PrintBoolean(value_t v)
{
	if(v.i == 1)
		printf("T\n");
	else
		printf("Nil\n");
}

static void PrintList(value_t v)
{
	printf("List\n");
}

static void PrintString(value_t v)
{
	printf("%s\n",String_Ptr(v)->s);
}

static value_t IncD(value_t v)
{
	return (value_t)(++v.d);
}
static value_t IncI(value_t v)
{
	return Int_init(++v.i);
}
static value_t DecD(value_t v)
{
	return (value_t)(--v.d);
}
static value_t DecI(value_t v)
{
	return Int_init(--v.i);
}

//Plus
static value_t PlusDD(value_t v2,value_t v1)
{
	return (value_t)(v2.d + v1.d);
}
static value_t PlusDI(value_t v2,value_t v1)
{
	return (value_t)(v2.d + v1.i);
}
static value_t PlusID(value_t v2,value_t v1)
{
	return (value_t)(v2.i + v1.d);
}
static value_t PlusII(value_t v2,value_t v1)
{
	return Int_init(v2.i + v1.i);
}

//Minus
static value_t MinusDD(value_t v2,value_t v1)
{
	return (value_t)(v2.d - v1.d);
}
static value_t MinusDI(value_t v2,value_t v1)
{
	return (value_t)(v2.d - v1.i);
}
static value_t MinusID(value_t v2,value_t v1)
{
	return (value_t)(v2.i - v1.d);
}
static value_t MinusII(value_t v2,value_t v1)
{
	return Int_init(v2.i - v1.i);
}

//Mul
static value_t MulDD(value_t v2,value_t v1)
{
	return (value_t)(v2.d * v1.d);
}
static value_t MulDI(value_t v2,value_t v1)
{
	return (value_t)(v2.d * v1.i);
}
static value_t MulID(value_t v2,value_t v1)
{
	return (value_t)(v2.i * v1.d);
}
static value_t MulII(value_t v2,value_t v1)
{
	return Int_init(v2.i * v1.i);
}

//Div
static value_t DivDD(value_t v2,value_t v1)
{
	return (value_t)(v2.d / v1.d);
}
static value_t DivDI(value_t v2,value_t v1)
{
	return (value_t)(v2.d / v1.i);
}
static value_t DivID(value_t v2,value_t v1)
{
	return (value_t)(v2.i / v1.d);
}
static value_t DivII(value_t v2,value_t v1)
{
	return Int_init(v2.i / v1.i);
}

//Mod
static value_t ModDD(value_t v2,value_t v1)
{
	return (value_t)(fmod(v2.d,v1.d));
}
static value_t ModDI(value_t v2,value_t v1)
{
	return (value_t)(fmod(v2.d,v1.i));
}
static value_t ModID(value_t v2,value_t v1)
{
	return (value_t)(fmod(v2.i , v1.d));
}
static value_t ModII(value_t v2,value_t v1)
{
	return Int_init(v2.i % v1.i);
}

//Lt
static value_t LtDD(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.d < v1.d);
	return t;
}
static value_t LtDI(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.d < v1.i);
	return t;
}
static value_t LtID(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.i < v1.d);
	return t;
}
static value_t LtII(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.i < v1.i);
	return t;
}

//Gt
static value_t GtDD(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.d > v1.d);
	return t;
}
static value_t GtDI(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.d > v1.i);
	return t;
}
static value_t GtID(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.i > v1.d);
	return t;
}
static value_t GtII(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.i > v1.i);
	return t;
}

//Eq
static value_t EqDD(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.d == v1.d);
	return t;
}
static value_t EqDI(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.d == v1.i);
	return t;
}
static value_t EqID(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.i == v1.d);
	return t;
}
static value_t EqII(value_t v2,value_t v1)
{
	value_t t;
	Boolean_init(t,v2.i == v1.i);
	return t;
}

static void(* Print[5])() = {
	PrintDouble,
	PrintInt,
	PrintBoolean,
	PrintList,
	PrintString,
};

static value_t(* Inc[2])() = { IncD, IncI };
static value_t(* Dec[2])() = { DecD, DecI };

static value_t(* Plus[2][2])() = {
	{ PlusDD, PlusDI },
	{ PlusID, PlusII }
};

static value_t(* Minus[2][2])() = {
	{ MinusDD, MinusDI },
	{ MinusID, MinusII }
};

static value_t(* Mul[2][2])() = {
	{ MulDD, MulDI },
	{ MulID, MulII }
};

static value_t(* Div[2][2])() = {
	{ DivDD, DivDI },
	{ DivID, DivII }
};

static value_t(* Mod[2][2])() = {
	{ ModDD, ModDI },
	{ ModID, ModII }
};

static value_t(* Lt[2][2])() = {
	{ LtDD, LtDI },
	{ LtID, LtII }
};

static value_t(* Gt[2][2])() = {
	{ GtDD, GtDI },
	{ GtID, GtII }
};

static value_t(* Eq[2][2])() = {
	{ EqDD, EqDI },
	{ EqID, EqII }
};

const void **vm_exec(bytecode_t *root,value_t st[],int esp,hash_table_t *hash,int table_flag)
{

	static const void *tables[] = {
		&&Label_Put,
		&&Label_SetHash,
		&&Label_LoadValue,
		&&Label_Inc,
		&&Label_Dec,
		&&Label_OpPlus,
		&&Label_OpMinus,
		&&Label_OpMul,
		&&Label_OpDiv,
		&&Label_OpMod,
		&&Label_OpLt,
		&&Label_OpGt,
		&&Label_OpEq,
		&&Label_OpCPlus,
		&&Label_OpCMinus,
		&&Label_OpCMul,
		&&Label_OpCDiv,
		&&Label_OpCMod,
		&&Label_OpCLt,
		&&Label_OpCGt,
		&&Label_OpCEq,
		&&Label_Print,
		&&Label_Call,
		&&Label_TJump,
		&&Label_Nop,
		&&Label_Args,
		&&Label_Ret
	};
	if(table_flag == 1){
		return tables;
	}
  

	int ebp = esp;
	bytecode_t *p = root;

	goto *p->iseq;
  
  Label_Put: {
		push(p->data);
		p = p->next;
//		printf("Put\n");
		goto *p->iseq;
	}
  Label_Inc: {
		value_t v = pop();
		push(Inc[Type_Check(v)](v));
		p = p->next;
//		printf("+c\n");
		goto *p->iseq;
	}
  Label_Dec: {
		value_t v = pop();
		push(Dec[Type_Check(v)](v));
		p = p->next;
		goto *p->iseq;
	}
  Label_OpPlus: {
		value_t v1 = pop();
		value_t v2 = pop();
		value_t ans = Plus[Type_Check(v2)][Type_Check(v1)](v2,v1);
		push(ans);
		p = p->next;
//		printf("+\n");
		goto *p->iseq;
	}
  Label_OpMinus: {
		value_t v1 = pop();
		value_t v2 = pop();
		value_t ans = Minus[Type_Check(v2)][Type_Check(v1)](v2,v1);
		push(ans);
		p = p->next;
//		printf("-\n");
		goto *p->iseq;
	}
  Label_OpMul: {
		value_t v1 = pop();
		value_t v2 = pop();
		value_t ans = Mul[Type_Check(v2)][Type_Check(v1)](v2,v1);
		push(ans);
		p = p->next;
//		printf("*\n");
		goto *p->iseq;
	}
  Label_OpDiv: {
		value_t v1 = pop();
		value_t v2 = pop();
		value_t ans = Div[Type_Check(v2)][Type_Check(v1)](v2,v1);
		push(ans);
		p = p->next;
//		printf("/\n");
		goto *p->iseq;
	}
  Label_OpMod: {
		value_t v1 = pop();
		value_t v2 = pop();
		value_t ans = Mod[Type_Check(v2)][Type_Check(v1)](v2,v1);
		push(ans);
		p = p->next;
//		printf("mod\n");
		goto *p->iseq;
	}
  Label_OpCPlus: {
		value_t v = pop();
		push(Plus[Type_Check(v)][Type_Check(p->data)](v,p->data));
		p = p->next;
//		printf("+c\n");
		goto *p->iseq;
	}
  Label_OpCMinus: {
		value_t v = pop();
		push(Minus[Type_Check(v)][Type_Check(p->data)](v,p->data));
		p = p->next;
//		printf("-c\n");
		goto *p->iseq;
	}
  Label_OpCMul: {
		value_t v = pop();
		push(Mul[Type_Check(v)][Type_Check(p->data)](v,p->data));
		p = p->next;
//		printf("*c\n");
		goto *p->iseq;
	}
  Label_OpCDiv: {
		value_t v = pop();
		push(Div[Type_Check(v)][Type_Check(p->data)](v,p->data));
		p = p->next;
//		printf("/c\n");
		goto *p->iseq;
	}
  Label_OpCMod: {
		value_t v = pop();
		push(Mod[Type_Check(v)][Type_Check(p->data)](v,p->data));
		p = p->next;
//		printf("modc\n");
		goto *p->iseq;
	}
  Label_Print: {
		value_t v = pop();
		Print[Type_Check(v)](v);
		p = p->next;
		goto *p->iseq;
	}
  Label_Ret: {
		value_t v = pop();
		st[ebp-1] = v;
//		printf("End\n");
		return NULL;
	}
  Label_OpLt: {
		value_t v1 = pop();
		value_t v2= pop();
		value_t ans = Lt[Type_Check(v2)][Type_Check(v1)](v2,v1);
		push(ans);
		p = p->next;
//		printf("<\n");
		goto *p->iseq;
	}
  Label_OpGt: {
		value_t v1 = pop();
		value_t v2 = pop();
		value_t ans = Gt[Type_Check(v2)][Type_Check(v1)](v2,v1);
		push(ans);
		p = p->next;
//		printf(">\n");
		goto *p->iseq;
	}
  Label_OpEq: {
		value_t v1 = pop();
		value_t v2 = pop();
		value_t ans = Eq[Type_Check(v2)][Type_Check(v1)](v2,v1);
		push(ans);
		p = p->next;
//		printf("=\n");
		goto *p->iseq;
	}
  Label_OpCLt: {
		value_t v = pop();
		value_t ans = Lt[Type_Check(v)][Type_Check(p->data)](v,p->data);
		push(ans);
		p = p->next;
//		printf("<c\n");
		goto *p->iseq;
	}
  Label_OpCGt: {
		value_t v = pop();
		value_t ans = Gt[Type_Check(v)][Type_Check(p->data)](v,p->data);
		push(ans);
		p = p->next;
//		printf(">\n");
		goto *p->iseq;
	}
  Label_OpCEq: {
		value_t v = pop();
		value_t ans = Eq[Type_Check(v)][Type_Check(p->data)](v,p->data);
		push(ans);
		p = p->next;
//		printf("=\n");
		goto *p->iseq;
	}
  Label_SetHash: {
		value_t a = pop();
		HashTable_insert_Value(hash, String_Ptr(p->data)->s, String_Ptr(p->data)->len, &a);
		p = p->next;
//		printf("sethash\n");
		goto *p->iseq;
	}
  Label_LoadValue: {
		value_t *b = HashTable_lookup_Value(hash, String_Ptr(p->data)->s,String_Ptr(p->data)->len);
		push(*b);
		p = p->next;
//		printf("LoadValue\n");
		goto *p->iseq;
	}
  Label_Call: {
		vm_exec( (bytecode_t *)p->data.o, st, esp, hash, 0);
		p = p->next;
//		printf("Call\n");
		goto *p->iseq;
	}
  Label_TJump: {
		p = (pop().bytes == True)? (bytecode_t *)p->data.o : p->next;
//		printf("TJump\n");
		goto *p->iseq;
	}
  Label_Nop: {
		p = p->next;
//		printf("Nop\n");
		goto *p->iseq;
	}
  Label_Args: {
		value_t v = st[ebp - p->data.i];
		push(v);
		p = p->next;
//		printf("Args\n");
		goto *p->iseq;
	}
 
	return NULL;
}

