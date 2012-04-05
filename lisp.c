#include "lisp.h"


void lisp_repl()
{
	printf("REPL cannot use now.\n");
}


void dumpCons_t(cons_t * p)
{
	if(p != NULL){
		if(p->type == TY_Car){
			printf("(\n");
			dumpCons_t(p->car);
		}else if(p->type == TY_Op){
			printf("%s\n",p->svalue);
		}else if(p->type == TY_Value){
			printf("%d\n",p->ivalue);
		}else if(p->type == TY_Cdr){
			printf(")\n");
		}
		if(p->cdr != NULL){
			dumpCons_t(p->cdr);
		}
	}
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

