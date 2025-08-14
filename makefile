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

all: tc vm hex-dump vm-lin

tc: tc.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o tc tc.c $(LDLIBS)
	ls -l tc

vm: vm.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o vm vm.c $(LDLIBS)
	ls -l vm

hex-dump: hex-dump.c
	$(CXX) $(CFLAGS) $(LDFLAGS) -o hex-dump hex-dump.c $(LDLIBS)
	ls -l hex-dump

bin: all
	cp -u -p tc ~/bin/
	cp -u -p vm ~/bin/
	cp -u -p hex-dump ~/bin/

# -------------------------------------------------------------------
# Scripts
# -------------------------------------------------------------------

clean:
	rm -f tc vm hex-dump a.out
	rm -f tc.out tc.sym vm.lst

test: all
	./tc test.tc
	./vm
	./hex-dump tc.out >> vm.lst

bm: all
	./tc bm.tc
	./vm
	./hex-dump tc.out >> vm.lst
