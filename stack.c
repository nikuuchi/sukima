#include "lisp.h"

#define stack_New() (stack_t *)malloc(sizeof(stack_t))

stack_cont_t *stack_cont_New()
{
	stack_cont_t *st = (stack_cont_t *)malloc(sizeof(stack_cont_t));
	return st;
}

stack_cont_t *stack_cont_init(value_t *vp){
	stack_cont_t *c = stack_cont_New();
	c->v = vp;
	return c;
}

stack_t *stack_init()
{
	stack_t *st = stack_New();
	st->size = 0;
	return st;
}

void freeStack(stack_t *self)
{
	while(self->size > 0){
		pop(self);
	}
	free(self);
}

value_t *pop(stack_t *self)
{
	if(self->size > 0){
		stack_cont_t *tmp = self->iterator;
		self->iterator = tmp->next;
		self->size -= 1;
		value_t *v = tmp->v;
		free(tmp);
		return v;
	}
	return NULL;
}

void push(stack_t *self,value_t *v)
{
	stack_cont_t *c = stack_cont_init(v);
	stack_cont_t *ptr = self->iterator;
	c->next = ptr;
	self->iterator = c;
	self->size += 1;
}
