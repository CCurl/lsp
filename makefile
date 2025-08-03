app := tc

ARCH ?= 32
CXX := clang
CFLAGS := -m$(ARCH) -O3 -D IS_LINUX

srcfiles := $(shell find . -name "*.c")
incfiles := $(shell find . -name "*.h")
LDLIBS   := -lm

all: tc vm

tc: tc.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o tc tc.c $(LDLIBS)
	ls -l tc

vm: vm-stk.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o vm vm-stk.c $(LDLIBS)
	ls -l vm

clean:
	rm -f tc vm

test: all
	./tc test.tc
	./vm

bm: all
	./tc bm.tc
	./vm

bin: $(app)
	cp -u -p tc ~/bin/
	cp -u -p vm ~/bin/
