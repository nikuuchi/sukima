
CXX = gcc
DEFS = -std=c99 
LIBS = 

SRCS = \
	lisp.c

OBJS = $(SRCS:.c=.o)
PROG = lisp

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) -g -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CXX) $(DEFS) -g -c -o $@ $<

clean:
	rm -f $(OBJS) $(PROG)

