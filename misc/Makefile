CC=gcc
CFLAGS=-Wall -O2 -g
LCFLAGS=-O2
LEX = flex
YACC=bison
LEXLIB = -lfl
YAS=./yas

all: yis yas hcl2c run_bus

# These are implicit rules for making .yo files from .ys files.
# E.g., make sum.yo
.SUFFIXES: .ys .yo
.ys.yo:
	$(YAS) $*.ys
	
# These are the explicit rules for making yis yas and hcl2c and hcl2v
yas-grammar.o: yas-grammar.c
	$(CC) $(LCFLAGS) -c yas-grammar.c

yas-grammar.c: yas-grammar.lex
	$(LEX) yas-grammar.lex
	mv lex.yy.c yas-grammar.c
	
cache.o: cache.c cache.h
	$(CC) $(CFLAGS) -c cache.c

lock.o: lock.c lock.h
	$(CC) $(CFLAGS) -c lock.c
	
bus.o: bus.c bus.h
	$(CC) $(CFLAGS) -c bus.c
	
shared_memory.o: shared_memory.c shared_memory.h
	$(CC) $(CFLAGS) -c shared_memory.c

isa.o: isa.c isa.h
	$(CC) $(CFLAGS) -c isa.c

yas.o: yas.c yas.h isa.h
	$(CC) $(CFLAGS) -c yas.c

yas: yas.o yas-grammar.o isa.o lock.o shared_memory.o bus.o cache.o
	$(CC) $(CFLAGS) yas-grammar.o yas.o isa.o lock.o shared_memory.o bus.o cache.o ${LEXLIB} -o yas -lpthread

yis.o: yis.c isa.h
	$(CC) $(CFLAGS) -c yis.c

yis: yis.o isa.o lock.o bus.o shared_memory.o cache.o
	$(CC) $(CFLAGS) yis.o isa.o lock.o bus.o shared_memory.o cache.o -o  yis -lpthread

hcl2c: hcl.tab.c lex.yy.c node.c outgen.c
	$(CC) $(LCFLAGS) node.c lex.yy.c hcl.tab.c outgen.c -o hcl2c

hcl2v: hcl.tab.c lex.yy.c node.c outgen.c
	$(CC) $(LCFLAGS) -DVLOG node.c lex.yy.c hcl.tab.c outgen.c -o hcl2v

hcl2u: hcl.tab.c lex.yy.c node.c outgen.c
	$(CC) $(LCFLAGS) -DUCLID node.c lex.yy.c hcl.tab.c outgen.c -o hcl2u

lex.yy.c: hcl.lex
	$(LEX) hcl.lex

hcl.tab.c: hcl.y
	$(YACC) -d hcl.y
	
run_bus.o: run_bus.c
	$(CC) $(CFLAGS) -c run_bus.c

run_bus: run_bus.o isa.o lock.o shared_memory.o bus.o cache.o
	$(CC) $(CFLAGS) run_bus.o isa.o lock.o shared_memory.o bus.o cache.o -o run_bus -lpthread -g

clean:
	rm -f *.o *.yo *.exe yis yas hcl2c mux4 *~ core.* 
	rm -f hcl.tab.c hcl.tab.h lex.yy.c


