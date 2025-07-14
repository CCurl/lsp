app := lsp

ARCH ?= 32
CXX := clang
CFLAGS := -m$(ARCH) -O3 -D IS_LINUX

srcfiles := $(shell find . -name "*.c")
incfiles := $(shell find . -name "*.h")
LDLIBS   := -lm

all: $(app) $(app32)

$(app): $(srcfiles) $(incfiles)
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $(app) $(srcfiles) $(LDLIBS)
	ls -l $(app)

clean:
	rm -f $(app) $(app32)

test: $(app)
	./$(app) test.lsp

run: $(app)
	./$(app)

bin: $(app)
	cp -u -p $(app) ~/bin/
