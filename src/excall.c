#include "lisp.h"

RETURN_VALUE SKM_Math_abs(value_t *st,int esp)
{
	RETURN_(Int_init(abs(pop().i)));
}

RETURN_VALUE SKM_Math_acos(value_t *st,int esp)
{
	RETURN_(Double_init(acos(pop().d)));
}

RETURN_VALUE SKM_Math_asin(value_t *st,int esp)
{
	RETURN_(Double_init(asin(pop().d)));
}

RETURN_VALUE SKM_Math_atan(value_t *st,int esp)
{
	RETURN_(Double_init(atan(pop().d)));
}

RETURN_VALUE SKM_Math_atan2(value_t *st,int esp)
{
	double x = pop().d;
	double y = pop().d;
	RETURN_(Double_init(atan2(y,x)));
}

RETURN_VALUE SKM_Math_ceil(value_t *st,int esp)
{
	RETURN_(Double_init(ceil(pop().d)));
}

RETURN_VALUE SKM_Math_cos(value_t *st,int esp)
{
	RETURN_(Double_init(cos(pop().d)));
}

RETURN_VALUE SKM_Math_cosh(value_t *st,int esp)
{
	RETURN_(Double_init(cosh(pop().d)));
}

RETURN_VALUE SKM_Math_exp(value_t *st,int esp)
{
	RETURN_(Double_init(exp(pop().d)));
}

RETURN_VALUE SKM_Math_fabs(value_t *st,int esp)
{
	RETURN_(Double_init(fabs(pop().d)));
}

RETURN_VALUE SKM_Math_floor(value_t *st,int esp)
{
	RETURN_(Double_init(floor(pop().d)));
}

RETURN_VALUE SKM_Math_log(value_t *st,int esp)
{
	RETURN_(Double_init(log(pop().d)));
}

RETURN_VALUE SKM_Math_log10(value_t *st,int esp)
{
	RETURN_(Double_init(log10(pop().d)));
}

RETURN_VALUE SKM_Math_pow(value_t *st,int esp)
{
	double multiplier = pop().d;
	double base = pop().d;
	RETURN_(Double_init(pow(base,multiplier)));
}

RETURN_VALUE SKM_Math_sin(value_t *st,int esp)
{
	RETURN_(Double_init(sin(pop().d)));
}

RETURN_VALUE SKM_Math_sinh(value_t *st,int esp)
{
	RETURN_(Double_init(sinh(pop().d)));
}

RETURN_VALUE SKM_Math_sqrt(value_t *st,int esp)
{
	RETURN_(Double_init(sqrt(pop().d)));
}

RETURN_VALUE SKM_Math_tan(value_t *st,int esp)
{
	RETURN_(Double_init(tan(pop().d)));
}

RETURN_VALUE SKM_Math_tanh(value_t *st,int esp)
{
	RETURN_(Double_init(tanh(pop().d)));
}

static skm_method_data data[] = {
	METHOD(abs,Math),
	METHOD(acos,Math),
	METHOD(atan,Math),
	METHOD(atan2,Math),
	METHOD(ceil,Math),
	METHOD(cos,Math),
	METHOD(cosh,Math),
	METHOD(exp,Math),
	METHOD(fabs,Math),
	METHOD(floor,Math),
	METHOD(log,Math),
	METHOD(log10,Math),
	METHOD(pow,Math),
	METHOD(sin,Math),
	METHOD(sinh,Math),
	METHOD(sqrt,Math),
	METHOD(tan,Math),
	METHOD(tanh,Math),

};

int excall(char *name,size_t name_len,value_t *st,int esp)
{
	int i = 0;
	while(strncmp(data[i].name,name,name_len) != 0) {
		++i;
	}
	return data[i].ex_method(st,esp);
}
