NSYSTEM=..
LDFLAGS= -g

CFLAGS= -std=c99 -g -I $(NSYSTEM)/include -I $(NSYSTEM)/src

test-subasta: test-subasta.o $(NSYSTEM)/lib/libnSys.a

clean:
	rm -rf *.o test-subasta
