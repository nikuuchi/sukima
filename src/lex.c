#include "lisp.h"

token_t *list_string_New()
{
	token_t *p = (token_t *)calloc(1,sizeof(token_t));
	p->str = NULL;
	p->next = NULL;
	return p;
}
void list_string_init(token_t *p,char *buf,size_t size,Type t)
{
	p->str = (char *)malloc(sizeof(char) * (size + 1));
	p->str = strncpy(p->str,buf,size);
	p->str[size] = '\0';
	p->size = sizeof(char) * size;
	if(t == TY_Str) {
		if(strcmp("defun",p->str) == 0) {
			t = TY_Defun;
		}else if(strcmp("if",p->str) == 0) {
			t = TY_If;
		}else if(strcmp("setq",p->str) == 0) {
			t = TY_Setq;
		}
	}
	p->type = t;
}

void freelist_string(token_t *p)
{
	if(p->next != NULL)
		freelist_string(p->next);
	if(p->type != TY_EOL) {
		free(p->str);
		p->str = NULL;
	}
	free(p);
	p = NULL;
}

token_t *lex(token_t *list,char * buf,int size)
{
	int index = 0;
	int next = 0;
	while(index < size) {
		switch(buf[index]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			++index;
			break;
		case '(':
			list_string_init(list,&buf[index],1,TY_Car);
			++index;
			if(index < size){
				list->next = list_string_New();
				list = list->next;
			}
			break;
		case ')':
			list_string_init(list,&buf[index],1,TY_Cdr);
			++index;
//			if(index < size){
			list->next = list_string_New();
			list = list->next;
//			}
			break;
		case '/':
		case '+':
		case '*':
		case '=':
		case '<':
		case '>':
			list_string_init(list,&buf[index],1,TY_Op);
			++index;
			if(index < size){
				list->next = list_string_New();
				list = list->next;
			}
			break;
		case '-':
			if(buf[index+1] == ' '){
				list_string_init(list,&buf[index],1,TY_Op);
				++index;
				if(index < size){
					list->next = list_string_New();
					list = list->next;
				}
			break;
			}
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			next = 1;
			while( isdigit(buf[index+next]) ) { ++next; }
			list_string_init(list,&buf[index],next,TY_Value);
			index += next;
//			if(index < size) {
				list->next = list_string_New();
				list = list->next;
//			}
			break;
		default:
			next = 1;
			while( isalpha(buf[index+next]) ) { ++next; }
			list_string_init(list,&buf[index],next,TY_Str);
			index += next;
//			if(index < size) {
				list->next = list_string_New();
				list = list->next;
//			}
			break;
		}
	}
	return list;
}

void dumpLexer(token_t *p)
{
	token_t *lex_current = p;
	while(lex_current != NULL){
		printf("%s:type:%d\n",lex_current->str,lex_current->type);
		lex_current = lex_current->next;
	}
}

void startLex(token_t *p,FILE *fp)
{
	char *buf;
	buf = (char *)malloc(sizeof(char) * BUF_SIZE);

	token_t *lex_current = p;
	while(fgets(buf,BUF_SIZE,fp)) {
		lex_current = lex(lex_current,buf,strlen(buf));
	}
	lex_current->type = TY_EOL;

	free(buf);
}
