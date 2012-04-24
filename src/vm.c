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

static void(* Print[2])() = {
	PrintDouble,
	PrintInt,
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
		int ans = v2->i + v1->i;
		value_t a;
		a.i = ans;
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpMinus: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		int ans = v2->i - v1->i;
		value_t a;
		a.i = ans;
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpMul: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		int ans = v2->i * v1->i;
		value_t a;
		a.i = ans;
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpDiv: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		int ans = v2->i / v1->i;
		value_t a;
		a.i = ans;
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_Print: {
		value_t *v = pop();
		Print[IS_Int(v)](v);
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
		int ans = v2->i < v1->i;
		value_t a;
		a.i = ans;
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpGt: {
		value_t *v1 = pop();
		value_t *v2 = pop();
		int ans = v2->i > v1->i;
		value_t a;
		a.i = ans;
		push(a);
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

