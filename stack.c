
#include "lisp.h"

#define STACK_SIZE 16384



stackp_t *stack_init()
{
	stackp_t *p = (stackp_t *)malloc(sizeof(stackp_t));
	p->st = (value_t **)calloc(STACK_SIZE, sizeof(value_t));
	p->esp = p->st[0];
	return p;
}


void push(stackp_t *self,value_t *p,value_t *esp)
{
	printf("push %d\n",p->num);
	self->esp = p;
	++(self->esp);
}

value_t *pop(stackp_t *self,value_t *esp)
{
	value_t *p = --esp;
	printf("-----pop %d\n",p->num);
	return p;
}

value_t *bsp(stackp_t *self,int n)
{
	int i = 0;
	value_t *b = self->esp;
	for(i = 0;i<n;++i)
		--b;
	return b;
}

void freeStack(stackp_t *self)
{
	int i = 0;
	value_t *p = self->st[0];
	for(i = 0;i<STACK_SIZE;++i){
		if(p != NULL){
			value_t *tmp = p;
			free(tmp);
			++p;
		}
	}
	free(self->st);
	free(self);
}
