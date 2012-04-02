#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024

#define Nil "Nil"
#define T 1
#define If "if"
#define Setq "setq"
#define Defun "defun"

typedef enum Type {TY_Car,TY_Cdr,TY_Op,TY_Value,TY_Nil,TY_Name} Type;

typedef struct cons_t{
  Type type;
  union {
    struct cons_t *car;
    int ivalue;
    char *svalue;
  };
  struct cons_t *cdr;
} cons_t;


void Cons_init(cons_t *Cell){


}

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

    while(fgets(buf,BUF_SIZE,fp)){
      printf("%s",buf);
    }

      fclose(fp);
  }else{
    printf("Too many arguments.");
  }
  
  return 0;
}



