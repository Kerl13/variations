.PHONY: all clean test

include config.mk

all: $(BUILD)variation

test: $(BUILD)tests/unrank
	$(BUILD)tests/unrank

clean:
	rm -rf $(BUILD)

# Libraries
include lib/xoshiro/xoshiro.mk
include lib/ibst/ibst.mk

# --- Executable ---

$(BUILD)variation: $(BUILD)libvar.a $(BUILD)libxoshiro.a $(BUILD)libibst.a
$(BUILD)variation: src/cli.c
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ src/cli.c -lvar -lxoshiro -lgmp -libst

# --- Static library ---

$(BUILD)libvar.a: $(BUILD)variation.o
	$(AR) rc $@ $?
	$(RANLIB) $@

# --- Object files ---

$(BUILD)variation.o: src/variation.c src/variation.h
$(BUILD)variation.o: lib/xoshiro/xoshiro256ss.h
$(BUILD)variation.o: lib/ibst/ibst.h
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ -c src/variation.c

# --- Tests ---

$(BUILD)tests/unrank: $(BUILD)libvar.a $(BUILD)libxoshiro.a $(BUILD)libibst.a
$(BUILD)tests/unrank: tests/unrank.c
	@mkdir -p $(BUILD)tests/
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ tests/unrank.c -lvar -lgmp -libst -lxoshiro
