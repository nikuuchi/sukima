#include "sukima.h"

skmObject SKM_Math_abs(skmObject *args)
{
	skmObject num = args[0];
	if(Type_Check(num) == 0) {
		return Double_init(fabs(num.d));
	}else {
		return Int_init(abs(num.i));
	}
}

skmObject SKM_Math_acos(skmObject *args)
{
	return Double_init(acos(args[0].d));
}

skmObject SKM_Math_asin(skmObject *args)
{
	return Double_init(asin(args[0].d));
}

skmObject SKM_Math_atan(skmObject *args)
{
	return Double_init(atan(args[0].d));
}

skmObject SKM_Math_atan2(skmObject *args)
{
	double y = args[0].d;
	double x = args[1].d;
	return Double_init(atan2(y,x));
}

skmObject SKM_Math_ceil(skmObject *args)
{
	return Double_init(ceil(args[0].d));
}

skmObject SKM_Math_cos(skmObject *args)
{
	return Double_init(cos(args[0].d));
}

skmObject SKM_Math_cosh(skmObject *args)
{
	return Double_init(cosh(args[0].d));
}

skmObject SKM_Math_exp(skmObject *args)
{
	return Double_init(exp(args[0].d));
}

skmObject SKM_Math_floor(skmObject *args)
{
	return Double_init(floor(args[0].d));
}

skmObject SKM_Math_log(skmObject *args)
{
	return Double_init(log(args[0].d));
}

skmObject SKM_Math_log10(skmObject *args)
{
	return Double_init(log10(args[0].d));
}

skmObject SKM_Math_pow(skmObject *args)
{
	double base = args[0].d;
	double multiplier = args[1].d;
	return Double_init(pow(base,multiplier));
}

skmObject SKM_Math_sin(skmObject *args)
{
	return Double_init(sin(args[0].d));
}

skmObject SKM_Math_sinh(skmObject *args)
{
	return Double_init(sinh(args[0].d));
}

skmObject SKM_Math_sqrt(skmObject *args)
{
	return Double_init(sqrt(args[0].d));
}

skmObject SKM_Math_tan(skmObject *args)
{
	return Double_init(tan(args[0].d));
}

skmObject SKM_Math_tanh(skmObject *args)
{
	return Double_init(tanh(args[0].d));
}

static skm_method_data data[] = {
	METHOD(Math,abs  ,1),
	METHOD(Math,acos ,1),
	METHOD(Math,atan ,1),
	METHOD(Math,atan2,2),
	METHOD(Math,ceil ,1),
	METHOD(Math,cos  ,1),
	METHOD(Math,cosh ,1),
	METHOD(Math,exp  ,1),
	METHOD(Math,floor,1),
	METHOD(Math,log  ,1),
	METHOD(Math,log10,1),
	METHOD(Math,pow  ,2),
	METHOD(Math,sin  ,1),
	METHOD(Math,sinh ,1),
	METHOD(Math,sqrt ,1),
	METHOD(Math,tan  ,1),
	METHOD(Math,tanh ,1),

};

int excall(char *name,size_t name_len,value_t *st,int esp)
{
	int i = 0;
	while(strncmp(data[i].name,name,name_len) != 0) { ++i; }
	skmObject args[10];
	int j = 0;
	for(;j<data[i].argc;++j) {
		args[data[i].argc - 1 - j] = pop();
	}
	push(data[i].ex_method(args));
	return esp;
}
