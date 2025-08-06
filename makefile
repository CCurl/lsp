app := tc

ARCH ?= 32
CXX := clang
CFLAGS := -m$(ARCH) -O3 -D IS_LINUX

srcfiles := $(shell find . -name "*.c")
incfiles := $(shell find . -name "*.h")
LDLIBS   := -lm

# -------------------------------------------------------------------
# Targets
# -------------------------------------------------------------------

all: tc vm-stk hex-dump gen-lin vm-lin

tc: tc.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o tc tc.c $(LDLIBS)
	ls -l tc

vm-stk: vm-stk.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o vm-stk vm-stk.c $(LDLIBS)
	ls -l vm-stk

vm-lin: vm-lin.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o vm-lin vm-lin.c $(LDLIBS)
	ls -l vm-lin

gen-lin: gen-lin.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o gen-lin gen-lin.c $(LDLIBS)
	ls -l gen-lin

hex-dump: hex-dump.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o hex-dump hex-dump.c $(LDLIBS)
	ls -l hex-dump

bin: all
	cp -u -p tc ~/bin/
	cp -u -p vm-stk ~/bin/
	cp -u -p hex-dump ~/bin/

# -------------------------------------------------------------------
# Scripts
# -------------------------------------------------------------------

clean:
	rm -f tc vm-stk gen-lin hex-dump a.out
	rm -f tc.out tc.sym vm-stk.lst

test: all
	./tc test.tc
	./vm-stk
	./hex-dump tc.out >> vm-stk.lst

lin-test: all
	./tc test.tc
	cat tc.out | ./gen-lin > a.out
	chmod +x a.out
	./hex-dump a.out
	./a.out

bm: all
	./tc bm.tc
	./vm-stk
	./hex-dump tc.out >> vm-stk.lst
