#include "lisp.h"


void vm_exec(list_run_t *root,stack_t *st,st_table_t *hash)
{
	list_run_t *p = root;
	while(p != NULL) {
		int ans = 0;
		int flag = 0;
		int i=0;
		switch(p->command) {
		case C_Put:
//			printf("put %d\n",p->v->num);
			push(st,p->v);
			break;
		case C_OptPlus:{
//			printf("OptPlus %d\n",p->v->num);
			ans = 0;
			for(i=0;i < p->v->num;++i) {
				value_t *v = pop(st);
				ans += v->num;
				if(v->type == Int_push){
					free(v);
				}
			}
			value_t *a = (value_t *)malloc(sizeof(value_t));
			a->type = Int_push;
			a->num = ans;
			push(st,a);
			break;
		}
		case C_OptMinus:{
//			printf("OptMinus %d\n",p->v->num);
			for(i=0;i < p->v->num -1 ;++i) {
				value_t *v = pop(st);
				ans -= v->num;
			}
			value_t *v = pop(st);
			ans += v->num;
			value_t *a = (value_t *)malloc(sizeof(value_t));
			a->type = Int_push;
			a->num = ans;
			push(st,a);
			break;
		}
		case C_OptMul:{
//			printf("OptMul %d\n",p->v->num);
			ans = 1;
			for(i=0;i < p->v->num ;++i) {
				value_t *v = pop(st);
				ans *= v->num;
				if(v->type == Int_push) {
					free(v);
				}
			}
			value_t *a = (value_t *)malloc(sizeof(value_t));
			a->type = Integer;
			a->num = ans;
			push(st,a);
			break;
		}
		case C_OptDiv: {
//			printf("OptDiv %d\n",p->v->num);
			ans = 1;
			for(i=0;i < p->v->num-1 ;++i) {
				value_t *v = pop(st);
				ans *= v->num;
				if(v->type == Int_push) {
					free(v);
				}
			}
			value_t *v = pop(st);
			ans = v->num / ans;
			value_t *a = (value_t *)malloc(sizeof(value_t));
			if(v->type == Int_push) {
				free(v);
			}
			a->type = Integer;
			a->num = ans;
			push(st,a);
			break;
		}
		case C_Print: {
			value_t *v = pop(st);
			printf("%d\n",v->num);
			push(st,v);
			break;
		}
		case C_End:
//			printf("End\n");
			break;
		case C_OptLt: {
//			printf("OptLt %d\n",p->v->num);
			ans = pop(st)->num;
			for(i=0;i < p->v->num - 1;++i) {
				value_t *v = pop(st);
				if(ans > v->num) {
					ans = v->num;
					flag = 1;
				}else{
					flag = 0;
					break;
				}
				if(v->type == Int_push) {
					free(v);
				}
			}
			value_t *a = (value_t *)malloc(sizeof(value_t));
			a->type = Boolean;
			a->num = flag;
			push(st,a);
			break;
		}
		case C_OptGt: {
//			printf("OptGt %d\n",p->v->num);
			ans = pop(st)->num;
			for(i=0;i < p->v->num - 1;++i) {
				value_t *v = pop(st);
				if(ans < v->num) {
					ans = v->num;
					flag = 1;
				}else{
					flag = 0;
					break;
				}
			}
			value_t *a = (value_t *)malloc(sizeof(value_t));
			a->type = Boolean;
			a->num = flag;
			push(st,a);
			break;
		}
		case C_PutObject: {
			value_t *a = pop(st);
//			printf("PutObject %s,%d\n",p->v->svalue,a->num);
			HashTable_insert_Value(hash,p->v->svalue,p->v->len, a);
			break;
		}
		case C_LoadValue: {
			value_t *b = HashTable_lookup_Value(hash,p->v->svalue,p->v->len);
//			printf("LoadValue %s,%d\n",p->v->svalue,b->num);
			push(st,b);
			break;
		}
		case C_Call: {
			list_run_t *func = HashTable_lookup_Function(hash,p->v->svalue,p->v->len);
//			printf("Call %s\n",p->v->svalue);
			hash = HashTable_createLocal(hash);
			vm_exec(func,st,hash);
			hash = HashTable_freeLocal(hash);
			break;
		}
		case C_TJump: {
			value_t *b = pop(st);
			int booleanflag = b->num;
			free(b);
			if(booleanflag == 1) {
//				printf("TJump %s,T\n",p->v->svalue);
			}else {
//				printf("TJump %s,Nil\n",p->v->svalue);
				char *str = p->v->svalue;
				while(p != NULL){
					if(p->command == C_Tag){
						if(strcmp(p->v->svalue,str) == 0){
//							printf("Tag %s\n",p->v->svalue);
							//free(str);
							goto jump;
						}
					}
					p = p->next;
				}
			}
			  jump:
			//free(b);
			break;
		}
		case C_Jump: {
//			printf("Jump %s\n",p->v->svalue);
			char *str = p->v->svalue;
			while(p != NULL){
				if(p->command == C_Tag){
					if(strcmp(p->v->svalue,str) == 0){
//						printf("Tag %s\n",p->v->svalue);
						goto jump2;
					}
				}
				p = p->next;
			}
		jump2:
			break;
		}
		case C_Tag:
//			printf("Tag %s\n",p->v->svalue);
			break;
		default:
//			printf("command:%d \n",p->command);
			break;
		}
		p = p->next;
	}
}

