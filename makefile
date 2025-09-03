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

all: tc fc vm hex-dump

tc: tc.c heap.c heap.h
	$(CXX) $(CFLAGS) $(LDFLAGS) -o tc tc.c heap.c $(LDLIBS)
	ls -l tc

fc: fc.c heap.c heap.h
	$(CXX) $(CFLAGS) $(LDFLAGS) -o fc fc.c heap.c $(LDLIBS)
	ls -l fc

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
	./tc test.tc > _test.asm
	fasm _test.asm test
	chmod +x test
	./hex-dump test > test.hex

ftest: all
	./fc ftest.tc > _ftest.asm
	fasm _ftest.asm ftest
	chmod +x ftest
	./hex-dump ftest > ftest.hex

bm: all
	./tc bm.tc > _bm.asm
	fasm _bm.asm bm
	chmod +x bm
	./hex-dump bm > bm.hex
