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

void list_string_init(list_string *p,char *buf,size_t size)
{
	p->next = (list_string *)malloc(sizeof(list_string));
	p->str = (char *)malloc(sizeof(char));
	p->str = strncpy(p->str,buf,size);
	p->size = sizeof(char);
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
		case ')':
		case '/':
		case '+':
		case '*':
		case '=':
		case '<':
		case '>':
			list_string_init(list,&buf[index],1);
			++index;
			list = list->next;
			break;
		case '-':
			if(buf[index+1] == ' '){
				list_string_init(list,&buf[index],1);
				++index;
				list = list->next;
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
			list_string_init(list,&buf[index],next);
			index += next;
			list = list->next;
			break;
		default:
			next = 1;
			while( isChar(buf[index+next]) ){ ++next; }			
			list_string_init(list,&buf[index],next);
			index += next;
			list = list->next;
			break;
		}
	}

	return list;
}
