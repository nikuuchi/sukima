#include "lisp.h"


int main(int argc, char *argv[])
{
	if(argc == 1){
		//REPL
	}else if(argc == 2){
		FILE *fp;
		char *buf;
		buf = (char *)malloc(sizeof(char) * BUF_SIZE);
		if((fp=fopen(argv[1],"r")) == NULL){
			printf("file open error\n");
			exit(0);
		}

		cons_t *root = (cons_t *)malloc(sizeof(cons_t));
		list_string *lex_buf = (list_string *)malloc(sizeof(list_string));
		list_string *lex_current = lex_buf;
		while(fgets(buf,BUF_SIZE,fp)){
			lex_current = lex(lex_current,buf,strlen(buf));
		}
		lex_current = lex_buf;
		while(lex_current != NULL){
			if(lex_current->str != NULL)
				printf("%s\n",lex_current->str);
			lex_current = lex_current->next;
		}
		freeCons_t(root);
		free(buf);
		fclose(fp);
	}else{
		printf("Too many arguments.");
	}
  
	return 0;
}

