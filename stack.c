#include "lisp.h"

stack *stack_init()
{
	stack *st = (stack *)malloc(sizeof(stack));
	return st;
}

void stack_destruct()
{

}

cons_t *pop(stack *self)
{
	if(self->next != NULL){
		stack *tmp = self->next;
		self->next = self->next->next;
		return tmp;
	}
	return NULL;
}

void push(stack *self,cons_t * p)
{
	stack *c = self;
	while(c->next != NULL){
		c = c->next;
	}
	c->next = (stack *)malloc(sizeof(stack));
}
