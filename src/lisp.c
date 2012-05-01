#include "lisp.h"

#define token_t_new() (token_t *)calloc(1,sizeof(token_t));

void lisp_repl()
{
	char *line;
	using_history();

	hash_table_t *hash = HashTable_init();
	int esp = 1;

	while((line = readline("Lisp>")) != NULL) {
		add_history(line);
		token_t *lex_buf =  (token_t *)malloc(sizeof(token_t));
		token_t *lex_current = lex_buf;

		lex_current = lex(lex_current,line,strlen(line));

		lex_current->type = TY_EOL;
		//dumpLexer(lex_buf);

		cons_t *root = Cons_New();
		parse(lex_buf,root);
		dumpCons_t(root); //debug
		printf("\n");

		bytecode_t *bytecode = Bytecode_New();
		//value_t st[128];
		compile(root,bytecode,hash);
		vm_exec(bytecode,esp,hash,0);
		Bytecode_free(bytecode);
		token_free(lex_buf);
		freeCons_t(root);
		free(line);
	}

	HashTable_free(hash);

}

void lisp_main(char *file)
{
	FILE *fp;
	if((fp=fopen(file,"r")) == NULL){
		printf("file open error\n");
		exit(0);
	}

	cons_t *root = Cons_New();
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
	bytecode_t *bytecode = Bytecode_New();
	hash_table_t *hash = HashTable_init();
	int esp = 1;
	//value_t st[1024];

	compile(root,bytecode,hash);
	vm_exec(bytecode,esp,hash,0);

	HashTable_free(hash);
	Bytecode_free(bytecode);
	token_free(lex_buf);
	freeCons_t(root);
	fclose(fp);
}


int main(int argc, char **argv)
{
	if(argc == 1) {
		lisp_repl();
	}else if(argc == 2) {
		lisp_main(argv[1]);
	}else{
		printf("Too many arguments.");
	}

	return 0;
}

