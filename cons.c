#include "lisp.h"


void freeCons_t(cons_t * p)
{
  if(p->car != NULL){
	  if(p->type == TY_Car){
		  freeCons_t(p->car);
	  }else if(p->type == TY_Op){
		  free(p->svalue);
	  }
  }
  if(p->cdr != NULL){
    freeCons_t(p->cdr);
  }
  free(p);
}


