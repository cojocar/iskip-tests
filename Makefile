
ifeq ($(V),)
Q := @
else
Q :=
endif

out:=./build
obj := $(out)
$(shell mkdir -p $(out))

CFLAGS += -fPIC
CFLAGS += -fshort-enums
CFLAGS += -g
CFLAGS += -fno-unwind-tables
CFLAGS += -fno-exceptions
export CFLAGS
#AR := arm-none-eabi-ar
#export AR

all: $(out)/libhello-full.a $(out)/libhmac-test-full.a $(out)/libbsearch-full.a $(out)/libaes-test-full.a

all-elf: $(out)/hello.elf $(out)/hmac-test.elf $(out)/bsearch.elf $(out)/aes-test.elf

### common targets
LIBCRYPTO := $(realpath ./third_party/cryptoc)
libcrypto:
	$(MAKE) obj=$(realpath $(out))/cryptoc -C $(LIBCRYPTO)
LDFLAGS_EXTRA += -L$(realpath $(out))/cryptoc -lcryptoc
EXTRA_AR += $(realpath $(out))/cryptoc/libcryptoc.a

LIBIO := $(realpath ./common/)
libio:
	$(MAKE) obj=$(realpath $(out))/libio -C $(LIBIO)
LDFLAGS_EXTRA += -L$(realpath $(out))/libio -lio-common
EXTRA_AR += $(realpath $(out))/libio/libio-common.a
#### endif

# stdlib or board specific library
ifeq ($(CONFIG_USE_BOARD),)
EXTRA_AR += $(realpath $(out))/libio/libio-stdlib.a
else
EXTRA_AR += $(realpath $(out))/libio/libio-board.a
endif


ifeq ($(CONFIG_USE_BOARD),)
	LDFLAGS_EXTRA += -lio-stdlib
else
	LDFLAGS_EXTRA += -lio-board
endif


### Hello World project
THIN_TARGETS += $(out)/libhello-full-thin.a
LIBHELO := $(realpath ./hello-world/)

libhello:
	$(MAKE) obj=$(realpath $(out))/libhello -C $(LIBHELO)

LDFLAGS_EXTRA += -L$(realpath $(out))/libhello -lhello
EXTRA_AR_HELLO := $(EXTRA_AR)
EXTRA_AR_HELLO += $(realpath $(out))/libhello/libhello.a

#this is the main target of this project
$(out)/libhello-full.a: libhello libio libcrypto
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) rcs $@ $(EXTRA_AR_HELLO)

## this is library is a thin one, not portable, but it can be used to generate the elf file
$(out)/libhello-full-thin.a: libhello libio libcrypto
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) rcT $@ $(EXTRA_AR_HELLO)

$(out)/hello.elf: $(out)/libhello-full-thin.a
	@echo "  LD      $(notdir $@)"
	$(Q)$(CC) $(LDFLAGS) -static $(CFLAGS) real_main.c -L$(out) -lhello-full-thin -o $@

####


### HMAC Test project
THIN_TARGETS += $(out)/libhmac-test-full-thin.a
LIBHMAC := $(realpath ./hmac/)

libhmac-test:
	$(MAKE) obj=$(realpath $(out))/libhmac-test -C $(LIBHMAC)

LDFLAGS_EXTRA += -L$(realpath $(out))/libhmac-test -lhmac-test
EXTRA_AR_HMAC := $(EXTRA_AR)
EXTRA_AR_HMAC += $(realpath $(out))/libhmac-test/libhmac-test.a

#this is the main target of this project
$(out)/libhmac-test-full.a: libhmac-test libio libcrypto
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) rcs $@ $(EXTRA_AR_HMAC)

## this is library is a thin one, not portable, but it can be used to generate the elf file
$(out)/libhmac-test-full-thin.a: libhmac-test libio libcrypto
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) rcT $@ $(EXTRA_AR_HMAC)

$(out)/hmac-test.elf: $(out)/libhmac-test-full-thin.a
	@echo "  LD      $(notdir $@)"
	$(Q)$(CC) $(LDFLAGS) -static $(CFLAGS) real_main.c -L$(out) -lhmac-test-full-thin -o $@
###

### AES Test project
THIN_TARGETS += $(out)/libaes-test-full-thin.a
LIBAES := $(realpath ./aes/)

libaes-test:
	$(MAKE) obj=$(realpath $(out))/libaes-test -C $(LIBAES)

LDFLAGS_EXTRA += -L$(realpath $(out))/libaes-test -laes-test
EXTRA_AR_AES := $(EXTRA_AR)
EXTRA_AR_AES += $(realpath $(out))/libaes-test/libaes-test.a

#this is the main target of this project
$(out)/libaes-test-full.a: libaes-test libio libcrypto
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) rcs $@ $(EXTRA_AR_AES)

## this is library is a thin one, not portable, but it can be used to generate the elf file
$(out)/libaes-test-full-thin.a: libaes-test libio libcrypto
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) rcT $@ $(EXTRA_AR_AES)

$(out)/aes-test.elf: $(out)/libaes-test-full-thin.a
	@echo "  LD      $(notdir $@)"
	$(Q)$(CC) $(LDFLAGS) -static $(CFLAGS) real_main.c -L$(out) -laes-test-full-thin -o $@
###

### BSearch
THIN_TARGETS += $(out)/libbsearch-full-thin.a
LIBBSEARCH := $(realpath ./bsearch/)

libbsearch:
	$(MAKE) obj=$(realpath $(out))/libbsearch -C $(LIBBSEARCH)

LDFLAGS_EXTRA += -L$(realpath $(out))/libbsearch -lbsearch
EXTRA_AR_BSEARCH := $(EXTRA_AR)
EXTRA_AR_BSEARCH += $(realpath $(out))/libbsearch/libbsearch.a

#this is the main target of this project
$(out)/libbsearch-full.a: libbsearch libio libcrypto
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) rcs $@ $(EXTRA_AR_BSEARCH)

## this is library is a thin one, not portable, but it can be used to generate the elf file
$(out)/libbsearch-full-thin.a: libbsearch libio libcrypto
	@echo "  AR      $(notdir $@)"
	$(Q)$(AR) rcT $@ $(EXTRA_AR_BSEARCH)

$(out)/bsearch.elf: $(out)/libbsearch-full-thin.a
	@echo "  LD      $(notdir $@)"
	$(Q)$(CC) $(LDFLAGS) -static $(CFLAGS) real_main.c -L$(out) -lbsearch-full-thin -o $@
###

CFLAGS += -I./common/

thin: $(THIN_TARGETS)

tmpdir := /tmp/itests-external-to-send

tar:
	$(Q)rm -rf $(tmpdir)
	$(Q)rsync -a -m \
		--exclude-from=internal.list \
		$(shell pwd) $(tmpdir)
	$(Q)rm -f $(shell pwd)/iskip-tests-$(shell git describe --tags --long).tar.gz
	$(Q)tar czvf $(shell pwd)/iskip-tests-$(shell git describe --tags --long).tar.gz -C $(tmpdir) .

build-zip:
	@echo "  ZIP     $(notdir $@)"
	$(Q)zip -r iskip-tests-build.`date +%s`.zip $(out)

.PHONY: clean
clean:
	@echo "  RM      $(out)"
	$(Q)rm -rf $(out)
	rm -f ./third_party/mini-printf/mini-printf.o
	rm -f *.tar.gz
	rm -f *.zip
