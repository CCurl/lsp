app := tc

ARCH ?= 32
CXX := clang
CFLAGS := -m$(ARCH) -O3 -D IS_LINUX

srcfiles := $(shell find . -name "*.c")
incfiles := $(shell find . -name "*.h")
LDLIBS   := -lm

all: tc vm-stk

tc: tc.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o tc tc.c $(LDLIBS)
	ls -l tc

vm-stk: vm-stk.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o vm-stk vm-stk.c $(LDLIBS)
	ls -l vm-stk

clean:
	rm -f tc vm-stk

test: all
	./tc test.tc
	./vm-stk

bm: all
	./tc bm.tc
	./vm-stk

bin: $(app)
	cp -u -p tc ~/bin/
	cp -u -p vm-stk ~/bin/
