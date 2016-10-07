obj ?= ./build
#CROSS_COMPILE ?=
#CC ?= $(CROSS_COMPILE)gcc
#AR ?= $(CROSS_COMPILE)ar

SOURCES = main.c

# Use V=1 for verbose output
ifeq ($(V),)
Q := @
else
Q :=
endif

CFLAGS += -I. -I../common -I../third_party/cryptoc/include/
CFLAGS += -Wall -Werror

OBJS = $(patsubst %.c,$(obj)/%.o,$(SOURCES))
DEPS = $(patsubst %.c,$(obj)/%.d,$(SOURCES))

# This is the default target
$(obj)/libhmac-test.a: $(OBJS)
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) scr $@ $^

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
