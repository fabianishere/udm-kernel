# Define default shell
SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
		else if [ -x /bin/bash ]; then echo /bin/bash; \
		else echo sh; fi; fi)

HAL_TOP := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

-include $(HAL_TOP)/file_list.mk
-include $(HAL_TOP)/file_list_samples.mk
-include $(HAL_TOP)/proprietary/file_list_samples_ex.mk

ifeq ($(AL_DEV_ID),)
OUTPUT = output/alpine_vx
HAL_PLATFORM_INCLUDE_PATH = $(HAL_PLATFORM_INCLUDE_PATH_ALPINE_V2)
HAL_PLATFORM_SOURCES = $(HAL_PLATFORM_SOURCES_ALPINE_V1) \
						$(HAL_PLATFORM_SOURCES_ALPINE_V2)
else
ifeq ($(AL_DEV_ID),AL_DEV_ID_ALPINE_V1)
OUTPUT = output/alpine_v1
HAL_PLATFORM_INCLUDE_PATH = $(HAL_PLATFORM_INCLUDE_PATH_ALPINE_V1)
HAL_PLATFORM_SOURCES = $(HAL_PLATFORM_SOURCES_ALPINE_V1)
else ifeq ($(AL_DEV_ID),AL_DEV_ID_ALPINE_V2)
OUTPUT = output/alpine_v2
HAL_PLATFORM_SOURCES = $(HAL_PLATFORM_SOURCES_ALPINE_V2)
HAL_PLATFORM_INCLUDE_PATH = $(HAL_PLATFORM_INCLUDE_PATH_ALPINE_V2)
else ifeq ($(AL_DEV_ID),AL_DEV_ID_ALPINE_V3)
ifeq ($(AL_DEV_REV_ID),1)
OUTPUT = output/alpine_v3
HAL_PLATFORM_INCLUDE_PATH = $(HAL_PLATFORM_INCLUDE_PATH_ALPINE_V3)
HAL_PLATFORM_SOURCES = $(HAL_PLATFORM_SOURCES_ALPINE_V3)
else
OUTPUT = output/alpine_v3_tc
HAL_PLATFORM_INCLUDE_PATH = $(HAL_PLATFORM_INCLUDE_PATH_ALPINE_V3)
HAL_PLATFORM_SOURCES = $(HAL_PLATFORM_SOURCES_ALPINE_V3)
endif
else ifeq ($(AL_DEV_ID),AL_DEV_ID_ALPINE_V4)
OUTPUT = output/alpine_v4
HAL_PLATFORM_INCLUDE_PATH = $(HAL_PLATFORM_INCLUDE_PATH_ALPINE_V4)
HAL_PLATFORM_SOURCES = $(HAL_PLATFORM_SOURCES_ALPINE_V4)
endif
endif

DEFINES = \
	-DAL_DDR_ERR_PRINTOUT_DISABLED=0 \
	-DAL_DDR_ASSERT_DISABLED=0 \
	-DAL_DDR_VERIF_SHORT_TIME=1 \
	-DAL_NAND_DEV_RESET=1 \
	-DAL_NAND_MUX_NAND8_CS0=1 \
	-DAL_DEV_ID_ALPINE_V1=0 \
	-DAL_DEV_ID_ALPINE_V2=1 \
	-DAL_DEV_ID_ALPINE_V3=2 \
	-DAL_DEV_ID_ALPINE_V4=3 \

ifneq ($(AL_DEV_ID),)
DEFINES += -DAL_DEV_ID=$(AL_DEV_ID)
endif

ifneq ($(AL_DEV_REV_ID),)
DEFINES += -DAL_DEV_REV_ID=$(AL_DEV_REV_ID)
endif

ifeq ($(AL_DEV_ID),AL_DEV_ID_ALPINE_V2)
ifneq ($(AL_HAL_EX),)
DEFINES += -DAL_ETH_EX
endif
endif

ifeq ($(AL_DEV_ID),AL_DEV_ID_ALPINE_V3)
ifneq ($(AL_HAL_EX),)
DEFINES += -DAL_ETH_EX
endif
endif

DESTDIR ?=
PREFIX ?= /usr/local
INSTALL = install

CC ?= gcc
CC := $(shell which $(CC) 2>/dev/null || type -p $(CC) || echo $(CC))

# Check GCC compiler for compile flag support
#
# @param compile flag
# @return provided compile flag if supported by compiler, empty otherwise
define gcc_check_flag
$(shell $(CC) $1 -E - < /dev/null > /dev/null 2>&1 && echo $1)
endef

# Notice: make sure these execute only once and not recursively
GCC_ARCH_CFLAGS := $(call gcc_check_flag,-mcpu=cortex-a15)
GCC_ARCH_CFLAGS += $(call gcc_check_flag,-mcpu=cortex-a57)
GCC_ARCH_CFLAGS += $(call gcc_check_flag,-mcpu=cortex-a72)
GCC_ARCH_CFLAGS += $(call gcc_check_flag,-marm)
GCC_ARCH_CFLAGS += $(call gcc_check_flag,-mfpu=neon-vfpv4)

ifeq ($(STACK_USAGE_FILE),1)
GCC_ARCH_CFLAGS += -fstack-usage
endif

ARCH_CFLAGS += $(GCC_ARCH_CFLAGS)

override CFLAGS += -MD -MP $(IDIR) $(DEFINES) $(ARCH_CFLAGS) -Wextra -Os -Wall -Werror \
	-Wstrict-prototypes -Wpointer-arith -Wnested-externs -Wcast-qual \
	-Wwrite-strings -Wformat-nonliteral -Wformat-security -Wendif-labels -g \
	-Wmissing-prototypes -Wredundant-decls -Wold-style-declaration \
	-Wold-style-definition -Wtype-limits -Wempty-body -Wformat-y2k\
	-Winit-self -Wignored-qualifiers -Wmissing-include-dirs

SOURCES += \
	$(HAL_DRIVER_SOURCES) \
	$(HAL_INIT_SOURCES_GENERIC) \
	$(HAL_PLATFORM_SOURCES)

SAMPLE_SOURCES += $(HAL_SAMPLE_SOURCES)

TESTS += \
	$(HAL_SAMPLE_TESTS) \

ifneq ($(AL_HAL_FP_EX),)
SOURCES += $(HAL_FP_EX_SOURCES_GENERIC)
endif

AL_HAL_PLAT_API_DIR ?= $(HAL_TOP)/include/plat_api/sample

IDIR += \
	-I$(AL_HAL_PLAT_API_DIR) \
	$(HAL_USER_INCLUDE_PATH) \
	$(HAL_DRIVER_INCLUDE_PATH) \
	$(HAL_INIT_INCLUDE_PATH) \
	$(HAL_PLATFORM_INCLUDE_PATH) \

#Add prorietary sources
ifneq ($(AL_HAL_EX), )
-include $(HAL_TOP)/proprietary/Makefile
endif

ifneq ($(AL_HAL_FP_EX),)
IDIR += $(HAL_FP_EX_INCLUDE_PATH)
endif

HAL_LIB = $(OUTPUT)/libhal.a
LDFLAGS += -L$(OUTPUT)/ -lhal

ifneq ($(AL_DUMMY_SYSCALLS),)
SOURCES += $(HAL_TOP)/samples/dummy_syscalls.S
LDFLAGS += -nostartfiles
endif

ARFLAGS = -rcs

OBJS = $(patsubst $(HAL_TOP)/%.c,$(OUTPUT)/%.o,$(filter %.c,$(SOURCES)))
OBJS += $(patsubst $(HAL_TOP)/%.S,$(OUTPUT)/%.o,$(filter %.S,$(SOURCES)))

SAMPLE_OBJS = $(patsubst $(HAL_TOP)/%.c,$(OUTPUT)/%.o,$(filter %.c,$(SAMPLE_SOURCES)))
SAMPLE_OBJS += $(patsubst $(HAL_TOP)/%.S,$(OUTPUT)/%.o,$(filter %.S,$(SAMPLE_SOURCES)))

TEST_OBJS = $(patsubst $(HAL_TOP)/%.c,$(OUTPUT)/%.o,$(filter %.c,$(TESTS)))
TEST_EXECS = $(patsubst $(HAL_TOP)/%.c,$(OUTPUT)/%.exe,$(filter %.c,$(TESTS)))
ifneq ($(EXT_OOT_SOURCES),)
EXT_OOT_OBJS := $(patsubst %.c,%.o,$(filter %.c,$(EXT_OOT_SOURCES)))
EXT_OOT_BIN ?= $(OUTPUT)/external/external_hal_app
else
EXT_OOT_OBJS=
EXT_OOT_BIN=
endif

###############################################################
#
# Verbose
#
###############################################################

ifeq ("$(origin V)", "command line")
VERBOSE := $(V)
else
VERBOSE = 0
endif

ifeq ($(VERBOSE),1)
quiet =
else
quiet = quiet_
endif

# make -s should not create any output
ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
quiet = silent_
endif

squote := '
escsq = $(subst $(squote),'\$(squote)',$1)

echo-cmd = $(if $($(quiet)cmd_$(1)),\
	echo '  $(call escsq,$($(quiet)cmd_$(1)))';)


###############################################################
#
# Commands
#
###############################################################

# Check if the invocation of make was with -B/--always-make
# this will force all special target to be rebuild
ifneq ($(filter -%B,$(MAKEFLAGS)),)
FORCE_BUILD := 1
else
FORCE_BUILD :=
endif

# Find any prerequisites that is newer than target
# Find any prerequisites that does not exists
# Find if the target itself doesnt exists
check_prereq = \
	$(filter-out FORCE,$?) \
	$(filter-out FORCE $(wildcard $^),$^) \
	$(filter-out FORCE $(wildcard $@),$@) \
	$(FORCE_BUILD)

# Check if the command line changed from previous build
check_cmd = \
	$(filter-out $(cmd_$@),$(cmd_$(1))) \
	$(filter-out $(cmd_$(1)),$(cmd_$@))

# Replace >$< with >$$< to preserve $ when reloading the .cmd file
# (needed for make)
# Replace >#< with >\#< to avoid starting a comment in the .cmd file
# (needed for make)
# Replace >'< with >'\''< to be able to enclose the whole string in '...'
# (needed for the shell
make_cmd = $(subst \#,\\\#,$(subst $$,$$$$,$(call escsq,$(cmd_$(1)))))

# saved command line file name
cmd_file_name = $(dir $(1)).$(notdir $(1)).cmd

# Makefile include files
include_cmd_files = $(wildcard $(foreach f,$(1),$(call cmd_file_name,$(f))))

target_rule = \
	$(if $(strip $(check_cmd) $(check_prereq)),\
		@set -e; \
		mkdir -p $(@D);	\
		echo 'cmd_$@ := $(make_cmd)' > $(call cmd_file_name,$@); \
		$(echo-cmd) \
		$(cmd_$(1)))

cc_o_c_rule = $(call target_rule,cc_o_c)
ifeq ($(filter %sparse,$(CC)),)
quiet_cmd_cc_o_c = CC      $@
else
quiet_cmd_cc_o_c = CHK     $@
endif
cmd_cc_o_c = $(CC) $(CFLAGS) -c -o $@ $<

cc_o_s_rule = $(call target_rule,cc_o_s)
ifeq ($(filter %sparse,$(CC)),)
quiet_cmd_cc_o_s = AS      $@
else
quiet_cmd_cc_o_s = CHK     $@
endif
cmd_cc_o_s = $(CC) $(CFLAGS) -c -o $@ $<

ld_rule = $(if $(filter %sparse,$(CC)),,$(call target_rule,ld))
quiet_cmd_ld = LD      $@
cmd_ld = $(CC) $(filter-out FORCE,$^) -o $@ $(LDFLAGS)

ar_rule = $(if $(filter %sparse,$(CC)),,$(call target_rule,ar))
quiet_cmd_ar = AR      $@
cmd_ar = $(AR) $(ARFLAGS) $@ $(filter-out FORCE,$^)

###############################################################
#
# Targets
#
###############################################################

.PHONY: all lib clean doc cscope FORCE install
.PRECIOUS: $(OBJS) $(SAMPLE_OBJS) $(TEST_OBJS) $(EXT_OOT_OBJS)

all: lib $(TEST_EXECS) $(EXT_OOT_BIN)

install: lib
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/lib
	$(INSTALL) -m 644 $(HAL_LIB) $(DESTDIR)$(PREFIX)/lib/

lib: $(HAL_LIB)

$(HAL_LIB): $(OBJS) FORCE
	$(call ar_rule)

$(EXT_OOT_BIN): $(EXT_OOT_OBJS) $(HAL_LIB) FORCE
	$(call ld_rule)

$(EXT_OOT_OBJS): %.o: %.c FORCE
	$(call cc_o_c_rule)

$(OUTPUT)/%.exe: $(SAMPLE_OBJS) $(HAL_LIB) $(OUTPUT)/%.o FORCE
	$(call ld_rule)

$(OUTPUT)/%.o: $(HAL_TOP)/%.c FORCE
	$(call cc_o_c_rule)

$(OUTPUT)/%.o: $(HAL_TOP)/%.S FORCE
	$(call cc_o_s_rule)

FORCE:

clean:
	rm -rf $(OUTPUT)

doc:
	@sed -i -re 's/(^.*@code) \*\//\1/' samples/*
	@(cat ./doc/Doxyfile ; echo "PROJECT_NUMBER=`cat version.txt`") | doxygen - 2>&1 | tee ./doc/doxy_log.txt
	@sed -i -re 's/(^.*@code)/\1 \*\//' samples/*
	@if [ -s ./doc/doxy_log.txt ]; then rm ./doc/doxy_log.txt; echo "Doxygen errors!" ; exit 1 ; fi;
	@rm ./doc/doxy_log.txt

cscope:
	git ls-tree -r HEAD --name-only  > cscope.files
	cscope -b -q -k

# pull in dependency info for *existing* .o files
-include $(OBJS:%.o=%.d)
-include $(TEST_OBJS:%.o=%.d)

# pull in command line info for *existing* .cmd files
include $(call include_cmd_files,\
		$(OBJS) $(TEST_OBJS) $(EXT_OOT_OBJS) \
		$(TEST_EXECS) $(EXT_OOT_BIN))
