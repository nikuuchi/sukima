#include "lisp.h"





void lisp_repl()
{
	printf("REPL cannot use now.\n");
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

	//--for debug
	lex_current = lex_buf;
	while(lex_current != NULL){
		if(lex_current->str != NULL)
			printf("%s:type:%d\n",lex_current->str,lex_current->type);
		lex_current = lex_current->next;
	}
	//--

	//I will add HashTable.
	parse(lex_buf,root);

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

