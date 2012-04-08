#include "lisp.h"

#define stack_cont_New() (stack_cont *)malloc(sizeof(stack_cont))

#define stack_New() (stack *)malloc(sizeof(stack))

stack_cont *stack_cont_init(value vp){
	stack_cont *c = stack_cont_New();
	c->v.num = vp.num;
	return c;
}

stack *stack_init()
{
	stack *st = stack_New();
	st->size = 0;
	return st;
}

void freeStack(stack *self)
{
	while(self->size > 0){
		pop(self);
	}
	free(self);
}

value pop(stack *self)
{
	if(self->size > 0){
		stack_cont *tmp = self->iterator;
		self->iterator = tmp->next;
		self->size -= 1;
		value v = tmp->v;
		free(tmp);
		return v;
	}
	value t;
	t.type = POP_NULL;
	return t;
}

void push(stack *self,value v)
{
	stack_cont *c = stack_cont_init(v);
	stack_cont *ptr = self->iterator;
	c->next = ptr;
	self->iterator = c;
	self->size += 1;	
}
