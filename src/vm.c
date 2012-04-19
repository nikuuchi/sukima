#include "lisp.h"

#define push(a) \
	do{ \
	st[esp]=(a); \
	/*printf("-------------push%d,%d\n",(a)->num,esp)*/; \
	++esp; \
	}while(0);

#define pop()(st[--esp])




void **vm_exec(list_run_t *root,value_t **st,int esp,st_table_t *hash,int table_flag)
{

	static void *tables[] = {
		&&Ia_Put,
		&&Ia_PutObject,
		&&Ia_LoadValue,
		&&Ia_OptPlus,
		&&Ia_OptMinus,
		&&Ia_OptMul,
		&&Ia_OptDiv,
		&&Ia_OptLt,
		&&Ia_OptGt,
		&&Ia_Print,
		&&Ia_Call,
		&&Ia_TJump,
		&&Ia_Jump,
		&&Ia_Tag,
		&&Ia_Args,
		&&Ia_End
	};
	if(table_flag == 1){
		return tables;
	}

	int bsp = esp;
	list_run_t *p = root;

	goto *p->iseq;

  Ia_Put: {
//	printf("put %d\n",p->v->num);
	push(p->v);
	p = p->next;
	goto *p->iseq;
	}
  Ia_OptPlus: {
//	printf("OptPlus %d\n",p->v->num);
	int ans = 0;
	int i = 0;
	int max = p->v->num;
	for(;i < max;++i){
		value_t *v = pop();
		ans += v->num;
		if(v->type == Int_push){
			free(v);
		}
	}
	value_t *a = (value_t*)malloc(sizeof(value_t));
	a->type = Int_push;
	a->num = ans;
	push(a);
	p = p->next;
	goto *p->iseq;
	}
  Ia_OptMinus: {
//	printf("OptMinus %d\n",p->v->num);
	int i = 0;
	int ans = 0;
	int max = p->v->num-1;
	for(;i < max;++i){
		value_t *v = pop();
		ans -= v->num;
		if(v->type == Int_push){
			free(v);
		}
	}
	value_t *v = pop();

	ans += v->num;
	value_t *a = (value_t*)malloc(sizeof(value_t));
	a->type = Int_push;
	a->num = ans;
	push(a);
	p = p->next;
	goto *p->iseq;
	}
  Ia_OptMul: {
	int ans = 1;
	int i = 0;
	int max = p->v->num;
	for(;i < max;++i){
		value_t *v = pop();
		ans *= v->num;
		if(v->type == Int_push){
			free(v);
		}
	}
	value_t *a = (value_t*)malloc(sizeof(value_t));
	a->type = Integer;
	a->num = ans;
//	printf("OptMul %d,%d\n",p->v->num,a->num);
	push(a);
	p = p->next;
	goto *p->iseq;
	}
  Ia_OptDiv: {
//	printf("OptDiv %d\n",p->v->num);
	int ans = 1;
	int i = 0;
	int max = p->v->num-1;
	for(;i < max;++i){
		value_t *v = pop();
		ans *= v->num;
		if(v->type == Int_push){
			free(v);
		}
	}
	value_t *v = pop();
	ans = v->num / ans;
	value_t *a = (value_t*)malloc(sizeof(value_t));
	if(v->type==Int_push){
		free(v);
	}
	a->type = Integer;
	a->num = ans;
	push(a);
	p = p->next;
	goto *p->iseq;
	}
  Ia_Print: {
	value_t *v = pop();
	printf("print %d\n",v->num);
	if(v->type == Int_push)
		free(v);
	p = p->next;
	goto *p->iseq;
	}
  Ia_End: {
	value_t *v = pop();
	value_t *f = st[bsp != 0?bsp-1:0];
	if(f != NULL)
		if(f->type == Int_push)
			free(f);
	st[bsp-1] = v;
	//printf("End\n");
	return NULL;
	}
  Ia_OptLt: {
//	printf("OptLt %d\n",p->v->num);
	int ans = pop()->num;
	int i = 0;
	int flag = 0;
	int max = p->v->num -1;
	for(;i < max;++i) {
		value_t *v = pop();
		if(ans > v->num) {
			ans = v->num;
			flag = 1;
		}else {
			flag = 0;
			break;
		}
		if(v->type == Int_push) {
			free(v);
		}
	}
	value_t *a = (value_t*)malloc(sizeof(value_t));
	a->type = Boolean;
	a->num = flag;
	push(a);
	p = p->next;
	goto *p->iseq;
	}
  Ia_OptGt: {
//	printf("OptGt %d\n",p->v->num);
	int ans = pop()->num;
	int i = 0;
	int flag = 0;
	int max = p->v->num - 1;
	for(;i < max;++i) {
		value_t *v = pop();
		if(ans < v->num) {
			ans = v->num;
			flag = 1;
		}else {
			flag = 0;
			break;
		}
	}
	value_t *a = (value_t*)malloc(sizeof(value_t));
	a->type = Boolean;
	a->num = flag;
	push(a);
	p = p->next;
	goto *p->iseq;
	}
  Ia_PutObject: {
	value_t *a = pop();
//	printf("PutObject %s,%d\n",p->v->string.s,a->num);
	HashTable_insert_Value(hash, p->v->string.s, p->v->string.len, a);
	p = p->next;
	goto *p->iseq;
	}
  Ia_LoadValue: {
	value_t *b = HashTable_lookup_Value(hash, p->v->string.s, p->v->string.len);
//	printf("LoadValue %s,%d\n",p->v->string.s,b->num);
	push(b);
	p = p->next;
	goto *p->iseq;
	}
  Ia_Call: {
		vm_exec( p->v->func, st, esp, hash, 0);
		p = p->next;
		goto *p->iseq;
	}
  Ia_TJump: {
		value_t *b = pop();
		int booleanflag = b->num;
		free(b);
		if(booleanflag == 1) {
			p = p->v->func;
			goto *p->iseq;
		}else {
			p = p->next;
			goto *p->iseq;
		}
	}
  Ia_Jump: {
//	printf("Jump %s\n",p->v->string.s);
	char *str = p->v->string.s;
	while(p != NULL){
		if(p->command == C_Tag){
			if(strcmp(p->v->string.s, str) == 0) {
//				printf("Tag %s\n",p->v->string.s);
				goto jump2;
			}
		}
		p = p->next;
	}
  jump2:
	p = p->next;
	goto *p->iseq;
	}
  Ia_Tag: {
		p = p->next;
		goto *p->iseq;
	}
  Ia_Args: {
	value_t *v = st[bsp - p->v->num];
//	printf("args: %d\n",v->num);
	push(v);
	p = p->next;
	goto *p->iseq;
	}
	return NULL;
}

