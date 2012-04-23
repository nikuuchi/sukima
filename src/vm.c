#include "lisp.h"

#define push_v(a) st[esp++].i = (a)->num
#define push(a) st[esp++] = (a)
#define pop() (&st[--esp])

void **vm_exec(command_t *root,stack_value_t st[],int esp,hash_table_t *hash,int table_flag)
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
		&&Label_Tag,
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
//	printf("put %d\n",p->v->num);
		push_v(p->v);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpPlus: {
//	printf("OpPlus %d\n",p->v->num);
		int ans = 0;
		int i = 0;
		int max = p->v->num;
		for(;i < max;++i){
			stack_value_t *v = pop();
			ans += v->i;
		}
		stack_value_t a;
		a.i = ans;
		push(a);

		p = p->next;
		goto *p->iseq;
	}
  Label_OpMinus: {
//	printf("OpMinus %d\n",p->v->num);
		int i = 0;
		int ans = 0;
		int max = p->v->num-1;
		for(;i < max;++i){
			stack_value_t *v = pop();
			ans -= v->i;
		}
		stack_value_t *v = pop();
		ans += v->i;
		stack_value_t a;
		a.i = ans;
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpMul: {
		int ans = 1;
		int i = 0;
		int max = p->v->num;
		for(;i < max;++i){
			stack_value_t *v = pop();
			ans *= v->i;
		}
		stack_value_t a;
		a.i = ans;
//	printf("OpMul %d,%d\n",p->v->num,a->num);
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpDiv: {
//	printf("OpDiv %d\n",p->v->num);
		int ans = 1;
		int i = 0;
		int max = p->v->num-1;
		for(;i < max;++i){
			stack_value_t *v = pop();
			ans *= v->i;
		}
		stack_value_t *v = pop();
		ans = v->i / ans;
		stack_value_t a;
		a.i = ans;
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_Print: {
		stack_value_t *v = pop();
		printf("print %d\n",v->i);
		p = p->next;
		goto *p->iseq;
	}
  Label_End: {
		stack_value_t *v = pop();
		st[bsp-1] = *v;
		//printf("End\n");
		return NULL;
	}
  Label_OpLt: {
//	printf("OpLt %d\n",p->v->num);
		int ans = pop()->i;
		int i = 0;
		int flag = 0;
		int max = p->v->num -1;
		for(;i < max;++i) {
			stack_value_t *v = pop();
			if(ans > v->i) {
				ans = v->i;
				flag = 1;
			}else {
				flag = 0;
				break;
			}
		}
		stack_value_t a;
		a.i = flag;
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_OpGt: {
//	printf("OpGt %d\n",p->v->num);
		int ans = pop()->i;
		int i = 0;
		int flag = 0;
		int max = p->v->num - 1;
		for(;i < max;++i) {
			stack_value_t *v = pop();
			if(ans < v->i) {
				ans = v->i;
				flag = 1;
			}else {
				flag = 0;
				break;
			}
		}
		stack_value_t a;
		a.i = flag;
		push(a);
		p = p->next;
		goto *p->iseq;
	}
  Label_SetHash: {
		stack_value_t *a = pop();
		value_t *b = (value_t *)malloc(sizeof(value_t));
		b->num = a->i;
//	printf("SetHash %s,%d\n",p->v->string.s,a->num);
		HashTable_insert_Value(hash, p->v->string.s, p->v->string.len, b);
		p = p->next;
		goto *p->iseq;
	}
  Label_LoadValue: {
		value_t *b = HashTable_lookup_Value(hash, p->v->string.s, p->v->string.len);
//	printf("LoadValue %s,%d\n",p->v->string.s,b->num);
		push_v(b);
		p = p->next;
		goto *p->iseq;
	}
  Label_Call: {
		vm_exec( p->v->func, st, esp, hash, 0);
		p = p->next;
		goto *p->iseq;
	}
  Label_TJump: {
		stack_value_t *b = pop();
		int booleanflag = b->i;
		if(booleanflag == 1) {
			p = p->v->func;
			goto *p->iseq;
		}else {
			p = p->next;
			goto *p->iseq;
		}
	}
  Label_Tag: {
		p = p->next;
		goto *p->iseq;
	}
  Label_Args: {
		stack_value_t v = st[bsp - p->v->num];
//	printf("args: %d\n",v->num);
		push(v);
		p = p->next;
		goto *p->iseq;
	}
	return NULL;
}

