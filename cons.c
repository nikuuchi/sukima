#include "lisp.h"


void freeCons_t(cons_t * p)
{
  if(p->car != NULL){
    freeCons_t(p->car);
  }
  if(p->cdr != NULL){
    freeCons_t(p->cdr);
  }
  free(p);
}


