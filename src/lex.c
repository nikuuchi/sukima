#include "lisp.h"

token_t *token_New()
{
	token_t *p = (token_t *)calloc(1,sizeof(token_t));
	p->str = NULL;
	p->next = NULL;
	return p;
}
void token_init(token_t *p,char *buf,size_t size,Type t)
{
	p->str = (char *)malloc(sizeof(char) * (size + 1));
	p->str = strncpy(p->str,buf,size);
	p->str[size] = '\0';
	p->size = sizeof(char) * size;
	if(t == TY_Str) {
		if(strcmp("defun",p->str) == 0) {
			t = TY_Defun;
		}else if(strcmp("T",p->str) == 0 || strcmp("NIL",p->str) == 0) {
			t = TY_Boolean;
		}else if(strcmp("if",p->str) == 0) {
			t = TY_If;
		}else if(strcmp("setq",p->str) == 0) {
			t = TY_Setq;
		}else if(strcmp("mod",p->str) == 0) {
			t = TY_Op;
		}else if(strcmp("eq",p->str) == 0) {
			t = TY_Op;
		}
	}
	p->type = t;
}

void token_free(token_t *p)
{
	if(p->next != NULL)
		token_free(p->next);
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
			if(buf[index+1] == ')') {
				++index;
				token_init(list,"NIL",sizeof("NIL"),TY_Str);
			}else {
				token_init(list,&buf[index],1,TY_LParen);
			}
			++index;
			list->next = token_New();
			list = list->next;
			break;
		case ')':
			token_init(list,&buf[index],1,TY_RParen);
			++index;
//			if(index < size){
			list->next = token_New();
			list = list->next;
//			}
			break;
		case '<':
		case '>':
			if(buf[index+1] == '='){
				token_init(list,&buf[index],2,TY_Op);
				index += 2;
				list->next = token_New();
				list = list->next;
				break;
			}
		case '/':
		case '+':
		case '*':
		case '=':
			token_init(list,&buf[index],1,TY_Op);
			++index;
			if(index < size){
				list->next = token_New();
				list = list->next;
			}
			break;
		case '-':
			if(buf[index+1] == ' '){
				token_init(list,&buf[index],1,TY_Op);
				++index;
				if(index < size){
					list->next = token_New();
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
			while ( isdigit(buf[index+next]) ) { ++next; }
			if(buf[index+next] == '.') {
				++next;
				while ( isdigit(buf[index+next]) ) { ++next; }
				token_init(list,&buf[index],next,TY_Double);
			}else {
				token_init(list,&buf[index],next,TY_Int);
			}
				index += next;
				list->next = token_New();
				list = list->next;
			break;
		case '\'':
		case '`':
			token_init(list,&buf[index],1,TY_List);
			++index;
			list->next = token_New();
			list = list->next;
			break;
		case '\"':
			next = 1;
			while( buf[index+next] != '\"'){ ++next; }
			token_init(list,&buf[index+1],next-1,TY_CStr);
			++next;
			index += next;
			list->next = token_New();
			list = list->next;
			break;
		case ';':
			return list;
		default:
			next = 1;
			while( isalpha(buf[index+next]) || isdigit(buf[index+next]) ) { ++next; }
			token_init(list,&buf[index],next,TY_Str);
			index += next;
			list->next = token_New();
			list = list->next;
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

