#include "lisp.h"


bool isInt(char c){
	if(c == '0' || c == '1'|| c == '2' ||
	   c == '3' || c == '4'|| c == '5' ||
	   c == '6' || c == '7'|| c == '8' || c == '9'){
		return true;
	}
	return false;
}

bool isChar(char c){
	if(c == ' ' || c == '\n'|| c == '\t' ||
	   c == '\0' || c == '\r'|| c == ')'|| c == '(' ){
		return false;
	}
	return true;
}

void list_string_init(list_string *p,char *buf,size_t size,Type t)
{
	p->str = (char *)malloc(sizeof(char));
	p->str = strncpy(p->str,buf,size);
	p->size = sizeof(char) * size;
	if(t == TY_Str){
		if(strcmp("defun",p->str) == 0){
			t = TY_Defun;
		}else if(strcmp("if",p->str) == 0){
			t = TY_If;
		}else if(strcmp("setq",p->str) == 0){
			t = TY_Setq;
		}
	}
	p->type = t;

}

void freelist_string(list_string *p){
	if(p->next != NULL)
		freelist_string(p->next);
	free(p->str);
	free(p);
}

list_string *lex(list_string *list,char * buf,int size)
{
	int index = 0;
	int next = 0;
	while(index < size){
		switch(buf[index]){
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
				list->next = (list_string *)malloc(sizeof(list_string));
				list = list->next;
			}
			break;
		case ')':
			list_string_init(list,&buf[index],1,TY_Cdr);
			++index;
			list->next = (list_string *)malloc(sizeof(list_string));
			if(index < size){
				list = list->next;
			}
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
				list->next = (list_string *)malloc(sizeof(list_string));
				list = list->next;
			}
			break;
		case '-':
			if(buf[index+1] == ' '){
				list_string_init(list,&buf[index],1,TY_Op);
				++index;
				if(index < size){
					list->next = (list_string *)malloc(sizeof(list_string));
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
			while( isInt(buf[index+next]) ){ ++next; }			
			list_string_init(list,&buf[index],next,TY_Value);
			index += next;
			if(index < size){
				list->next = (list_string *)malloc(sizeof(list_string));
				list = list->next;
			}
			break;
		default:
			next = 1;
			while( isChar(buf[index+next]) ){ ++next; }
			list_string_init(list,&buf[index],next,TY_Str);
			index += next;
			if(index < size){
				list->next = (list_string *)malloc(sizeof(list_string));
				list = list->next;
			}
			break;
		}
	}
	return list;
}
