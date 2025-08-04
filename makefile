app := tc

ARCH ?= 32
CXX := clang
CFLAGS := -m$(ARCH) -O3 -D IS_LINUX

srcfiles := $(shell find . -name "*.c")
incfiles := $(shell find . -name "*.h")
LDLIBS   := -lm

all: tc vm-stk hex-dump

tc: tc.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o tc tc.c $(LDLIBS)
	ls -l tc

vm-stk: vm-stk.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o vm-stk vm-stk.c $(LDLIBS)
	ls -l vm-stk

hex-dump: hex-dump.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o hex-dump hex-dump.c $(LDLIBS)
	ls -l hex-dump

clean:
	rm -f tc vm-stk

test: all
	./tc test.tc
	./vm-stk
	./hex-dump tc.out >> vm-stk.lst

bm: all
	./tc bm.tc
	./vm-stk
	./hex-dump tc.out >> vm-stk.lst

bin: $(app)
	cp -u -p tc ~/bin/
	cp -u -p vm-stk ~/bin/
	cp -u -p hex-dump ~/bin/
