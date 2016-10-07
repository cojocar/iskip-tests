obj ?= ./build

SOURCES = io-common.c

ifeq ($(CONFIG_USE_BOARD),)
SOURCES_STDLIB += io-stdlib.c
else
include board-flags.mk
endif

ifeq ($(V),)
Q := @
else
Q :=
endif

CFLAGS += -I../third_party/mini-printf/
CFLAGS += -Wall -Werror

OBJS = $(patsubst %.c,$(obj)/%.o,$(SOURCES))
OBJS +=$(obj)/mini-printf.o
DEPS = $(patsubst %.c,$(obj)/%.d,$(SOURCES))

ALL_TARGETS=$(obj)/libio-common.a

ifeq ($(CONFIG_USE_BOARD),)
OBJS_EXTRA = $(patsubst %.c,$(obj)/%.o,$(SOURCES_STDLIB))
DEPS_EXTRA = $(patsubst %.c,$(obj)/%.d,$(SOURCES_STDLIB))
EXTRA_TARGET = $(obj)/libio-stdlib.a
else
OBJS_EXTRA = $(patsubst %.c,$(obj)/%.o,$(SOURCES_BOARD))
DEPS_EXTRA = $(patsubst %.c,$(obj)/%.d,$(SOURCES_BOARD))
EXTRA_TARGET = $(obj)/libio-board.a
endif
ALL_TARGETS += $(EXTRA_TARGET)

all: $(ALL_TARGETS)

$(obj)/libio-common.a: $(OBJS)
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) scr $@ $^

$(EXTRA_TARGET): $(OBJS_EXTRA)
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) scr $@ $^

$(obj):
	@echo "  MKDIR   $(obj)"
	$(Q)mkdir -p $(obj)

$(obj)/mini-printf.o: ../third_party/mini-printf/mini-printf.c | $(obj)
	@echo "  CC      $(notdir $<)"
	$(Q)$(CC) $(CFLAGS) -c -MMD -MF $(basename $@).d -o $(basename $@).o $<

$(obj)/%.d $(obj)/%.o: %.c | $(obj)
	@echo "  CC      $(notdir $<)"
	$(Q)$(CC) $(CFLAGS) -c -MMD -MF $(basename $@).d -o $(basename $@).o $<

.PHONY: clean
clean:
	@echo "  RM      $(obj)"
	$(Q)rm -rf $(obj)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
-include $(DEPS_EXTRA)
endif
