# --- Static library ---

$(BUILD)libibst.a: $(BUILD)ibst.o
	$(AR) rc $@ $?
	$(RANLIB) $@

# --- Object files ---

$(BUILD)ibst.o: lib/ibst/ibst.c lib/ibst/ibst.h
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ -c lib/ibst/ibst.c
