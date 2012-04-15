
CC = gcc
DEFS = 
LIBS = 
CFLAGS = -Wall -g 

SRCS = \
	vm.c \
	runtime.c \
	parser.c \
	lex.c \
	cons.c \
	hash.c \
	lisp.c

OBJS = $(SRCS:.c=.o)
PROG = lisp

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(DEFS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(PROG)

