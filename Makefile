TESTS :=
TESTS += t/malloc

#all: gremlin.dylib gremlin.so
all: gremlin.so example
clean:
	rm -f example gremlin.so gremlin.dylib
	rm -f gremlin.o example.o
	rm -f $(TESTS) $(TESTS=:.o)

test: gremlin.so $(TESTS)
	./t/run

#gremlin.dylib: gremlin.o
#	$(CC) $(LDFLAGS) -shared -undefined dynamic_lookup -o $@ $+

gremlin.so: gremlin.o
	$(CC) $(LDFLAGS) -shared -o $@ $+ -ldl

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c -o $@ $+
