#include "lisp.h"

int eval_plus(cons_t *p);
int eval_minus(cons_t *p);
int eval_mul(cons_t *p);
int eval_div(cons_t *p);

int eval(cons_t *p)
{
	if(p != NULL){
		if(p->type == TY_Car){
			return eval(p->car);
		}else if(p->type == TY_Op){
			if(p->svalue[0] == '+'){
				return eval_plus(p->cdr);
			}else if(p->svalue[0] == '-'){
				return eval_minus(p->cdr);
			}else if(p->svalue[0] == '*'){
				return eval_mul(p->cdr);
			}else if(p->svalue[0] == '/'){
				return eval_div(p->cdr);
			}
		}else if(p->type == TY_Value){
			return p->ivalue;
		}else if(p->type == TY_Cdr){
			return 0;
		}
	}	
	return 0;
}

int eval_plus(cons_t *p)
{
	cons_t *c = p;
	int a = 0;
	while(c->type != TY_Cdr){
		int tmp = eval(c);
		if(c->type == TY_Cdr)
			break;
		a += tmp;
		c = c->cdr;
	}
	return a;
}
int eval_minus(cons_t *p)
{
	cons_t *c = p;
	int a = eval(c);
	c = c->cdr;
	while(c->type != TY_Cdr){
		int tmp = eval(c);
		if(c->type == TY_Cdr)
			break;
		a -= tmp;
		c = c->cdr;
	}
	return a;

}
int eval_mul(cons_t *p)
{
	cons_t *c = p;
	int a = 1;
	while(c->type != TY_Cdr){
		int tmp = eval(c);
		if(c->type == TY_Cdr)
			break;
		a *= tmp;
		c = c->cdr;
	}
	return a;
}
int eval_div(cons_t *p)
{
	cons_t *c = p;
	int a = eval(c);
	c = c->cdr;
	while(c->type != TY_Cdr){
		int tmp = eval(c);
		if(c->type == TY_Cdr)
			break;
		a /= tmp;
		c = c->cdr;
	}
	return a;

}


