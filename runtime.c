#include "lisp.h"

#define ListRun_New() (list_run *)malloc(sizeof(list_run))

#define setValue(c,d)							\
	(c)->command = C_Put;						\
	(c)->v.type = Integer;						\
	(c)->v.num = (d);

#define setBoolean(c,d)							\
	(c)->command = C_Put;						\
	(c)->v.type = Boolean;						\
	(c)->v.svalue = (d);

#define setOpt(c,d,e)							\
	(c)->command = (d);							\
	(c)->v.type = Integer;						\
	(c)->v.num = (e);


list_run *asm_Car(list_run *cmd,cons_t *cu);
list_run *asm_Op(list_run *cmd,cons_t *cu);
list_run *asm_UseFunction(list_run *cmd,cons_t *cu);
list_run *assemble(list_run *p,cons_t *cu);
void execute(list_run *root);

void freeListRun(list_run *p){
	if(p != NULL){
		freeListRun(p->next);
	}
	free(p);
}

void run(cons_t *ast)
{
	cons_t *chain = ast;
	list_run *root = ListRun_New();
	list_run *p = root;
	while(chain->cdr != NULL){
		switch(chain->type){
		case TY_Car:
			p = assemble(p,chain->car);
			break;
		case TY_Value:
			setValue(p,chain->ivalue);
			break;
		default:
			printf("some error occured in run().\n");
			break;			
		}
		chain = chain->cdr;	
	}

	execute(root);
	freeListRun(root);
}

list_run *assemble(list_run *p,cons_t *cu)
{
	switch(cu->type){
	case TY_Cdr:
		setBoolean(p,"Nil");
		break;
	case TY_Op:
		p = asm_Op(p,cu);
		break;
	case TY_EOL:
		p->command = C_End;
		break;
	case TY_Str:
		p = asm_UseFunction(p,cu);
		break;
	default:
		printf("some error occured in assemble().\n");
		break;
	}
	return p;
}

list_run *asm_Car(list_run *cmd,cons_t *cu)
{
	return asm_Op(cmd,cu->car);
}

list_run *asm_Op(list_run *cmd,cons_t *cu)
{
	int count = 0;
	cons_t *p = cu->cdr;
	list_run *list = cmd;

	while(p->type != TY_Cdr){
		switch(p->type){
		case TY_Car:
			list = asm_Car(list,p);
			break;
		case TY_Value:
			setValue(list,p->ivalue);
			break;
		default:
			printf("some error occured in asm_Op().\n");
			break;
		}
		list->next = ListRun_New();
		list = list->next;
		++count;
		p = p->cdr;
	}
	
    switch(cu->svalue[0]){
	case '+':
		setOpt(list,C_OptPlus,count);
		break;
	case '-':
		setOpt(list,C_OptMinus,count);
		break;
	case '*':
		setOpt(list,C_OptMul,count);
		break;
	case '/':
		setOpt(list,C_OptDiv,count);
		break;
	case '<':
		setOpt(list,C_OptLt,count);
		break;
	case '>':
		setOpt(list,C_OptGt,count);
		break;
	default:
		printf("some error occured in asm_Op().\n");
		break;
	}
	list->next = ListRun_New();
	list->next->command = C_End;
	return list;
}

list_run *asm_UseFunction(list_run *cmd,cons_t *cu)
{
	int count = 0;
	cons_t *p = cu->cdr;
	list_run *list = cmd;

	while(p->type != TY_Cdr){
		switch(p->type){
		case TY_Car:
			list = asm_Car(list,p);
			break;
		case TY_Value:
			setValue(list,p->ivalue);
			break;
		default:
			printf("some error occured in asm_UseFunction().\n");
			break;
		}

		list->next = ListRun_New();
		list = list->next;
		++count;
		p = p->cdr;
	}

	if(strcmp(cu->svalue,"print") == 0){
		list->command = C_Print;
		list->v.type = CALLFUNCTION;
		list->v.svalue = "print";
	}else{
		list->command = C_Call;
		list->v.type = CALLFUNCTION;
		list->v.num = count;
		list->v.svalue = cu->svalue;
	}

	
	list->next = ListRun_New();
	list->next->command = C_End;
	return list;
}

void execute(list_run *root)
{
	list_run *p = root;
	stack *st = stack_init();
	while(p != NULL){
		int ans = 0;
		value a;
		int flag = 0;
		int i=0;
		switch(p->command){
		case C_Put:
			printf("put %d\n",p->v.num);
			push(st,p->v);
			break;
		case C_OptPlus:
			printf("OptPlus %d\n",p->v.num);
			ans = 0;
			for(i=0;i < p->v.num;++i){
				value v = pop(st);
				ans += v.num;
			}
			a.type = Integer;
			a.num = ans;
			push(st,a);
			break;
		case C_OptMinus:
			printf("OptMinus %d\n",p->v.num);			
			for(i=0;i < p->v.num -1 ;++i){
				value v = pop(st);
				ans -= v.num;
			}
			ans += pop(st).num;
			a.type = Integer;
			a.num = ans;
			push(st,a);
			break;
		case C_OptMul:
			printf("OptMul %d\n",p->v.num);			
			ans = 1;
			for(i=0;i < p->v.num ;++i){
				value v = pop(st);
				ans *= v.num;
			}
			a.type = Integer;
			a.num = ans;
			push(st,a);
			break;
		case C_OptDiv:
			printf("OptDiv %d\n",p->v.num);
			ans = 1;
			for(i=0;i < p->v.num-1 ;++i){
				value v = pop(st);
				ans *= v.num;
			}
			ans = pop(st).num / ans;
			a.type = Integer;
			a.num = ans;
			push(st,a);
			break;
		case C_Print:
			printf("print value = %d\n",pop(st).num);
			break;
		case C_End:
			printf("End\n");			
			break;
		case C_OptLt:
			printf("OptLt %d\n",p->v.num);
			ans = pop(st).num;
			for(i=0;i < p->v.num - 1;++i){
				value v = pop(st);
				if(ans > v.num){
					ans = v.num;
					flag = 1;
				}else{
					flag = 0;
					break;
				}
			}
			a.type = Boolean;
			a.num = flag;
			push(st,a);
			break;
		case C_OptGt:
			printf("OptGt %d\n",p->v.num);
			ans = pop(st).num;
			for(i=0;i < p->v.num - 1;++i){
				value v = pop(st);
				if(ans < v.num){
					ans = v.num;
					flag = 1;
				}else{
					flag = 0;
					break;
				}
			}
			a.type = Boolean;
			a.num = flag;
			push(st,a);
			break;
		default:
			printf("command:%d \n",p->command);
			break;
		}
		p = p->next;
	}
	freeStack(st);
}
