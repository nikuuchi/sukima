#include "lisp.h"

void lisp_repl()
{
	printf("REPL cannot use now.\n");
}

void lisp_main(char *file,size_t size)
{
	FILE *fp;
	if((fp=fopen(file,"r")) == NULL){
		printf("file open error\n");
		exit(0);
	}

	cons_t *root = (cons_t *)malloc(sizeof(cons_t));
	list_string *lex_buf = (list_string *)malloc(sizeof(list_string));

	//--Lexer
	startLex(lex_buf,fp);
	//dumpLexer(lex_buf);

	//--Parser
	parse(lex_buf,root);
	dumpCons_t(root); //debug

	//--eval
	//printf("\n--answer:%d\n",eval(root)); //AST

	//--run
	printf("\n");
	run(root);

	freelist_string(lex_buf);
	freeCons_t(root);
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

