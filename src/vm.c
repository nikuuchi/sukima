#include "lisp.h"

#define push(a) st[esp++] = (a)
#define pop() (&st[--esp])

static void PrintInt(value_t *v)
{
	printf("%d\n",v->i);
}
static void PrintDouble(value_t *v)
{
	printf("%f\n",v->d);
}


//Plus
static value_t PlusDD(value_t *v2,value_t *v1)
{
	return (value_t)(v2->d + v1->d);
}
static value_t PlusDI(value_t *v2,value_t *v1)
{
	return (value_t)(v2->d + v1->i);
}
static value_t PlusID(value_t *v2,value_t *v1)
{
	return (value_t)(v2->i + v1->d);
}
static value_t PlusII(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->i + v1->i);
	return t;
}

//Minus
static value_t MinusDD(value_t *v2,value_t *v1)
{
	return (value_t)(v2->d - v1->d);
}
static value_t MinusDI(value_t *v2,value_t *v1)
{
	return (value_t)(v2->d - v1->i);
}
static value_t MinusID(value_t *v2,value_t *v1)
{
	return (value_t)(v2->i - v1->d);
}
static value_t MinusII(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->i - v1->i);
	return t;
}

//Mul
static value_t MulDD(value_t *v2,value_t *v1)
{
	return (value_t)(v2->d * v1->d);
}
static value_t MulDI(value_t *v2,value_t *v1)
{
	return (value_t)(v2->d * v1->i);
}
static value_t MulID(value_t *v2,value_t *v1)
{
	return (value_t)(v2->i * v1->d);
}
static value_t MulII(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->i * v1->i);
	return t;
}

//Div
static value_t DivDD(value_t *v2,value_t *v1)
{
	return (value_t)(v2->d / v1->d);
}
static value_t DivDI(value_t *v2,value_t *v1)
{
	return (value_t)(v2->d / v1->i);
}
static value_t DivID(value_t *v2,value_t *v1)
{
	return (value_t)(v2->i / v1->d);
}
static value_t DivII(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->i / v1->i);
	return t;
}

//Lt
static value_t LtDD(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->d < v1->d);
	return t;
}
static value_t LtDI(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->d < v1->i);
	return t;
}
static value_t LtID(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->i < v1->d);
	return t;
}
static value_t LtII(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->i < v1->i);
	return t;
}

//Gt
static value_t GtDD(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->d > v1->d);
	return t;
}
static value_t GtDI(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->d > v1->i);
	return t;
}
static value_t GtID(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->i > v1->d);
	return t;
}
static value_t GtII(value_t *v2,value_t *v1)
{
	value_t t;
	Int_init(t,v2->i > v1->i);
	return t;
}

static void(* Print[2])() = {
	PrintDouble,
	PrintInt
};

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

static value_t(* Lt[2][2])() = {
	{ LtDD, LtDI },
	{ LtID, LtII }
};

static value_t(* Gt[2][2])() = {
	{ GtDD, GtDI },
	{ GtID, GtII }
};


void **vm_exec(command_t *root,value_t st[],int esp,hash_table_t *hash,int table_flag)
{

	static void *tables[] = {
		&&Label_Put,
		&&Label_SetHash,
		&&Label_LoadValue,
		&&Label_OpPlus,
		&&Label_OpMinus,
		&&Label_OpMul,
		&&Label_OpDiv,
		&&Label_OpLt,
		&&Label_OpGt,
		&&Label_Print,
		&&Label_Call,
		&&Label_TJump,
		&&Label_Nop,
		&&Label_Args,
		&&Label_End
	};
	if(table_flag == 1){
		return tables;
	}

	int bsp = esp;
	command_t *p = root;

	goto *p->iseq;

  Label_Put: {
		push(p->data[0]);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpPlus: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		value_t ans = Plus[Is_Int(v2)][Is_Int(v1)](v2,v1);
		push(ans);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpMinus: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		value_t ans = Minus[Is_Int(v2)][Is_Int(v1)](v2,v1);
		push(ans);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpMul: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		value_t ans = Mul[Is_Int(v2)][Is_Int(v1)](v2,v1);
		push(ans);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpDiv: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		value_t ans = Div[Is_Int(v2)][Is_Int(v1)](v2,v1);
		push(ans);
		p = p->next;
		goto *p->iseq;
	}
  Label_Print: {
		value_t *v = pop();
		Print[Is_Int(v)](v);
		p = p->next;
		goto *p->iseq;
	}
  Label_End: {
		value_t *v = pop();
		st[bsp-1] = *v;
		//printf("End\n");
		return NULL;
	}
  Label_OpLt: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		value_t ans = Lt[Is_Int(v2)][Is_Int(v1)](v2,v1);
		push(ans);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpGt: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		value_t ans = Gt[Is_Int(v2)][Is_Int(v1)](v2,v1);
		push(ans);
		p = p->next;
		goto *p->iseq;
	}
  Label_SetHash: {
		value_t *a = pop();
		value_t *b = (value_t *)malloc(sizeof(value_t));
		b = a;
//	printf("SetHash %s,%d\n",p->v->string.s,a->num);
		HashTable_insert_Value(hash, p->data[0].string->s, p->data[0].string->len, b);
		p = p->next;
		goto *p->iseq;
	}
  Label_LoadValue: {
		value_t *b = HashTable_lookup_Value(hash, p->data[0].string->s, p->data[0].string->len);
//	printf("LoadValue %s,%d\n",p->v->string.s,b->num);
		push(*b);
		p = p->next;
		goto *p->iseq;
	}
  Label_Call: {
		vm_exec( (command_t *)p->data[0].pointer, st, esp, hash, 0);
		p = p->next;
		goto *p->iseq;
	}
  Label_TJump: {
		p = (pop()->i == 1)? (command_t *)p->data[0].pointer : p->next;
		goto *p->iseq;
	}
  Label_Nop: {
		p = p->next;
		goto *p->iseq;
	}
  Label_Args: {
		value_t v = st[bsp - p->data[0].i];
//	printf("args: %d\n",data[0].i);
		push(v);
		p = p->next;
		goto *p->iseq;
	}
	return NULL;
}

