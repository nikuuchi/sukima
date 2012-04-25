#include "lisp.h"

#define token_t_new() (token_t *)calloc(1,sizeof(token_t));

void lisp_repl()
{
	char *line;
	using_history();

	hash_table_t *hash = HashTable_init();
	int esp = 0;

	while((line = readline("Lisp> ")) != NULL) {
		add_history(line);
		token_t *lex_buf =  (token_t *)malloc(sizeof(token_t));
		token_t *lex_current = lex_buf;

		lex_current = lex(lex_current,line,strlen(line));
		lex_current->type = TY_EOL;
		//dumpLexer(lex_buf);

		cons_t *root = (cons_t *)malloc(sizeof(cons_t));
		parse(lex_buf,root);
		dumpCons_t(root); //debug
		printf("\n");

		command_t *bytecode = Command_New();
		value_t st[8192];
		compile(root,bytecode,hash);
		vm_exec(bytecode,st,esp,hash,0);

		Command_free(bytecode);
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
	token_t *lex_buf = (token_t *)malloc(sizeof(token_t));

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
	command_t *bytecode = Command_New();
	hash_table_t *hash = HashTable_init();
	int esp = 0;
	value_t st[8192];

	compile(root,bytecode,hash);
	vm_exec(bytecode,st,esp,hash,0);

	HashTable_free(hash);
	Command_free(bytecode);
	freelist_string(lex_buf);
	freeCons_t(root);
	fclose(fp);
}


int main(int argc, char **argv)
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

