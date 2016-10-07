obj ?= ./build

SOURCES = main.c

# Use V=1 for verbose output
ifeq ($(V),)
Q := @
else
Q :=
endif

BSEARCH_ELEM_TYPE ?= uint16_t

CFLAGS += -I. -I../common
CFLAGS += -Wall -Werror
CFLAGS += -Delem_type=$(BSEARCH_ELEM_TYPE)

OBJS = $(patsubst %.c,$(obj)/%.o,$(SOURCES))
DEPS = $(patsubst %.c,$(obj)/%.d,$(SOURCES))

OBJS += $(obj)/bsearch_gen.o

# This is the default target
$(obj)/libbsearch.a: $(OBJS)
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) scr $@ $^

bsearch_gen: .FORCE
	@echo "  GEN     $(notdir $@).c"
	$(Q)BSEARCH_ELEM_TYPE=$(BSEARCH_ELEM_TYPE) python bsearch_gen.py > $(obj)/bsearch_gen.c

$(obj)/bsearch_gen.o: bsearch_gen
	@echo "  CC      $(notdir $<).o"
	$(Q)$(CC) $(CFLAGS) -c -MMD -MF $(obj)/bsearch_gen.d -o $(obj)/bsearch_gen.o $(obj)/bsearch_gen.c


$(obj):
	@echo "  MKDIR   $(obj)"
	$(Q)mkdir -p $(obj)

$(obj)/%.d $(obj)/%.o: %.c | $(obj)
	@echo "  CC      $(notdir $<)"
	$(Q)$(CC) $(CFLAGS) -c -MMD -MF $(basename $@).d -o $(basename $@).o $<


.PHONY: clean
clean:
	@echo "  RM      $(obj)"
	$(Q)rm -rf $(obj)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.FORCE:
