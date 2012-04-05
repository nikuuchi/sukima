#include "lisp.h"


void lisp_repl()
{
	printf("REPL cannot use now.\n");
}


void dumpCons_t(cons_t * p)
{
	if(p != NULL){
		if(p->type == TY_Car){
			printf("( ");
			dumpCons_t(p->car);
		}else if(p->type == TY_Op){
			printf("%s ",p->svalue);
		}else if(p->type == TY_Value){
			printf("%d ",p->ivalue);
		}else if(p->type == TY_Cdr){
			printf(") ");
		}
		if(p->cdr != NULL){
			dumpCons_t(p->cdr);
		}
	}
}

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


void lisp_main(char *file,size_t size)
{
	FILE *fp;
	char *buf;
	buf = (char *)malloc(sizeof(char) * BUF_SIZE);
	if((fp=fopen(file,"r")) == NULL){
		printf("file open error\n");
		exit(0);
	}

	cons_t *root = (cons_t *)malloc(sizeof(cons_t));
	list_string *lex_buf = (list_string *)malloc(sizeof(list_string));
	list_string *lex_current = lex_buf;
	while(fgets(buf,BUF_SIZE,fp)){
		lex_current = lex(lex_current,buf,strlen(buf));
	}
	lex_current->type = TY_EOL;

	//--for debug
	lex_current = lex_buf;
	while(lex_current != NULL){
		if(lex_current->type != TY_EOL){
			printf("%s:type:%d\n",lex_current->str,lex_current->type);
		}else{
			printf("EOL\n");
		}
		lex_current = lex_current->next;
	}
	//--

	//I will add HashTable.
	parse(lex_buf,root);
	printf("----parse\n");
	dumpCons_t(root); //debug

	printf("\n%d\n",eval(root)); //AST

	freelist_string(lex_buf);
	freeCons_t(root);
	free(buf);
	fclose(fp);
}


int main(int argc, char *argv[])
{
	if(argc == 1){
		lisp_repl();
	}else if(argc == 2){
		lisp_main(argv[1],strlen(argv[1]));
	}else{
		printf("Too many arguments.");
	}
  
	return 0;
}

