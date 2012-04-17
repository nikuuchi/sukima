#include "lisp.h"

#define list_string_t_new() (list_string_t *)calloc(1,sizeof(list_string_t));

void lisp_repl()
{
	char *line;
	using_history();

	st_table_t *hash = HashTable_init();
	int esp = 0;
	value_t **st = (value_t **)calloc(8192,sizeof(value_t));

	while((line = readline("Lisp> ")) != NULL) {
		add_history(line);
		list_string_t *lex_buf =  (list_string_t *)malloc(sizeof(list_string_t));
		list_string_t *lex_current = lex_buf;

		lex_current = lex(lex_current,line,strlen(line));
		lex_current->type = TY_EOL;
		dumpLexer(lex_buf);

		cons_t *root = (cons_t *)malloc(sizeof(cons_t));
		parse(lex_buf,root);
		dumpCons_t(root); //debug
		printf("\n");

		list_run_t *bytecode = ListRun_New();
		compile(root,bytecode,hash);
		vm_exec(bytecode,st,esp,hash,0);

		freeListRun(bytecode);
		freelist_string(lex_buf);
		freeCons_t(root);
		free(line);
	}

	HashTable_free(hash);
}

void lisp_main(char *file,size_t size)
{
	FILE *fp;
	if((fp=fopen(file,"r")) == NULL){
		printf("file open error\n");
		exit(0);
	}

	cons_t *root = (cons_t *)malloc(sizeof(cons_t));
	list_string_t *lex_buf = (list_string_t *)malloc(sizeof(list_string_t));

	//--Lexer
	startLex(lex_buf,fp);
	dumpLexer(lex_buf);

	//--Parser
	parse(lex_buf,root);
	dumpCons_t(root); //debug

	//--eval
	//printf("\n--answer:%d\n",eval(root)); //AST

	//--run
	printf("\n");
	list_run_t *bytecode = ListRun_New();
	st_table_t *hash = HashTable_init();
	int esp = 0;
	value_t **st = (value_t **)calloc(8192,sizeof(value_t));

	compile(root,bytecode,hash);
	vm_exec(bytecode,st,esp,hash,0);

	HashTable_free(hash);
	//freeStack(st);
	//free(st);
	freeListRun(bytecode);
	freelist_string(lex_buf);
	freeCons_t(root);
	fclose(fp);
}


int main(int argc, char *argv[])
{
	if(argc == 1) {
		lisp_repl();
	}else if(argc == 2) {
		lisp_main(argv[1],strlen(argv[1]));
	}else{
		printf("Too many arguments.");
	}
  
	return 0;
}

