
CC = gcc
DEFS = -std=c99 
LIBS = 
CFLAGS = -g -Wall

SRCS = \
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

