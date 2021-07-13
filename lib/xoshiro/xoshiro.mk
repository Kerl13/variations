# --- Static library ---

$(BUILD)libxoshiro.a: $(BUILD)xoshiro256ss.o
	$(AR) rc $@ $?
	$(RANLIB) $@

# --- Object files ---

$(BUILD)xoshiro256ss.o: lib/xoshiro/xoshiro256ss.c lib/xoshiro/xoshiro256ss.h
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ -c lib/xoshiro/xoshiro256ss.c
