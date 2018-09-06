# lowercase function
lc = $(shell echo $(1) | tr A-Z a-z)

# compiler executables
CLANG_EXEC = clang
GCC_EXEC = gcc

# clang warnings we ignore
CLANG_IGNORED_WARNINGS = reserved-id-macro                 \
			 variadic-macros                   \
			 undef                             \
			 gnu-zero-variadic-macro-arguments \
			 documentation                     \
			 padded                            \
			 missing-noreturn                  \
			 unused-parameter                  \
			 covered-switch-default            \
			 disabled-macro-expansion          \
			 pedantic

# flags passed to clang for warnings
CLANG_WARNING_FLAGS = -Weverything $(addprefix -Wno-,$(CLANG_IGNORED_WARNINGS))

# gcc warnings we ignore
GCC_IGNORED_WARNINGS = packed-bitfield-compat \
		       unused-parameter       \
		       cast-function-type

# flags passed to gcc for warnings
GCC_WARNING_FLAGS = -Wall -Wextra $(addprefix -Wno-,$(GCC_IGNORED_WARNINGS))

# flags passed to the selected compiler for warnings
CC_WARNING_FLAGS = $($(COMPILER)_WARNING_FLAGS)

# flag passed for the c language standard
CC_STD_FLAG = -std=gnu11

# symbols to define under each mode
CC_COMMON_DEFINES := GNU_SOURCE ASSERT_RECOVER UNREACHABLE_RECOVER
CC_DEBUG_DEFINES := DEBUG_ENABLE
CC_OPT_DEFINES :=

ifndef ASSERT
CC_COMMON_DEFINES += DISABLE_ASSERT
endif

ifndef UNREACHABLE
CC_COMMON_DEFINES += DISABLE_UNREACHABLE
endif

ifdef SHODDY_CPU_TIMINGS
CC_COMMON_DEFINES += DISABLE_CYCLECHECK SHODDY_CPU_TIMINGS
endif

CC_RELEASE_DEFINES := ASSERT_ASSUME UNREACHABLE_ASSUME DISABLE_CYCLECHECK

# defines for the selected mode
CC_DEFINES = $(CC_COMMON_DEFINES) $(CC_$(MODE)_DEFINES)
CC_DEFINES_FLAGS = $(addprefix -D,$(CC_DEFINES))

EMU_INCLUDE_DIR = $(EMU_SRC_DIR)/include
CC_INCLUDE_FLAG = -I$(EMU_INCLUDE_DIR)

# libs the emulator has to link with
EMU_LIBS = SDL2 readline
CC_LIB_FLAGS = $(addprefix -l,$(EMU_LIBS))

# flag passed for LTO
CC_LTO_FLAG = -flto

# flag for the target architecture
CC_ARCH_FLAG = -march=native

# opt-in ubsan
ifdef UBSAN
COMMON_SANITIZER_FLAGS += -fsanitize=undefined
CLANG_SANITIZER_FLAGS += -fsanitize=nullability
endif

# opt-in asan
ifdef ASAN
COMMON_SANITIZER_FLAGS += -fsanitize=address
endif

CC_SANITIZER_FLAGS = $(COMMON_SANITIZER_FLAGS) $($(COMPILER)_SANITIZER_FLAGS)

# flags given for each compilation mode
CC_COMMON_FLAGS = $(CC_WARNING_FLAGS) $(CC_STD_FLAG) $(CC_DEFINES_FLAGS) $(CC_INCLUDE_FLAG) $(CC_ARCH_FLAG)
CC_DEBUG_FLAGS = $(CC_COMMON_FLAGS) $(CC_SANITIZER_FLAGS) -g -O1 -fno-inline -fno-omit-frame-pointer -fno-optimize-sibling-calls
CC_OPT_FLAGS = $(CC_COMMON_FLAGS) $(CC_SANITIZER_FLAGS) -g -O3
CC_RELEASE_FLAGS = $(CC_COMMON_FLAGS) $(CC_LTO_FLAG) -g -O3

# final flags and command for chosen compiler and mode
CC_FLAGS = $(CC_$(MODE)_FLAGS)
CC_EXEC = $($(COMPILER)_EXEC)
CC_COMMAND = $(CC_EXEC) $(CC_FLAGS)

# allow setting a custom CC65_PREFIX
ifdef CC65_PREFIX
CC65_EXEC = $(CC65_PREFIX)/cc65
CA65_EXEC = $(CC65_PREFIX)/ca65
AR65_EXEC = $(CC65_PREFIX)/ar65
LD65_EXEC = $(CC65_PREFIX)/ld65
else
CC65_EXEC = cc65
CA65_EXEC = ca65
AR65_EXEC = ar65
LD65_EXEC = ld65
endif

# source directories
EMU_SRC_DIR = emu
LIB_SRC_DIR = lib
TEST_SRC_DIR = test

# base library bundled with Hawknest
BASELIB = sim6502.lib

# custom linker config
LD65_CFG_DIR = cfg
LD65_CFG = $(LD65_CFG_DIR)/hawknest.cfg

# build artifact locations
BUILD_DIR = build
EMU_BUILD_DIR = $(BUILD_DIR)/emu/$(call lc,$(COMPILER))/$(call lc,$(MODE))
LIB_BUILD_DIR = $(BUILD_DIR)/lib
TEST_BUILD_DIR = $(BUILD_DIR)/test

# binary destinations
BIN_DIR = bin
EMU = $(BIN_DIR)/hawknest-$(call lc,$(COMPILER))-$(call lc,$(MODE))
LIB = $(BIN_DIR)/hawknest.lib
TESTS = $(addprefix $(BIN_DIR)/,$(patsubst %.c,%,$(patsubst %.s,%,$(TEST_SRC))))

# sources included via modules.mk files
EMU_SRC :=
include $(EMU_SRC_DIR)/modules.mk

LIB_SRC :=
include $(LIB_SRC_DIR)/modules.mk

TEST_SRC :=
include $(TEST_SRC_DIR)/modules.mk

## names of object and dependency files ##
EMU_OBJ = $(addprefix $(EMU_BUILD_DIR)/,$(EMU_SRC:.c=.o))
EMU_DEP = $(EMU_OBJ:.o=.d)

LIB_CASM = $(addprefix $(LIB_BUILD_DIR)/,$(patsubst %.c,%.s,$(filter %.c,$(LIB_SRC))))
TEST_CASM = $(addprefix $(TEST_BUILD_DIR)/,$(patsubst %.c,%.s,$(filter %.c,$(TEST_SRC))))

LIB_COBJ = $(LIB_CASM:.s=.o)
TEST_COBJ = $(TEST_CASM:.s=.o)

LIB_ASMOBJ = $(addprefix $(LIB_BUILD_DIR)/,$(patsubst %.s,%.o,$(filter %.s,$(LIB_SRC))))
TEST_ASMOBJ = $(addprefix $(TEST_BUILD_DIR)/,$(patsubst %.s,%.o,$(filter %.s,$(TEST_SRC))))

LIB_OBJ = $(LIB_COBJ) $(LIB_ASMOBJ)
TEST_OBJ = $(TEST_COBJ) $(TEST_ASMOBJ)

LIB_DEP = $(LIB_OBJ:.o=.d)
TEST_DEP = $(TEST_OBJ:.o=.d)

# default COMPILER and MODE
COMPILER = GCC
MODE = DEBUG

## phony targets ##
emu: $(EMU)

lib: $(LIB)

tests: $(TESTS)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: emu lib tests clean

# find and include depfiles
include $(wildcard $(EMU_DEP) $(LIB_DEP) $(TEST_DEP))

## generic rules ##
$(EMU_OBJ): $(EMU_BUILD_DIR)/%.o: $(EMU_SRC_DIR)/%.c
	@echo "$@ <- $<"
	@mkdir -p $(dir $@)
	@$(CC_COMMAND) -MD -MP -c $< -o $@

$(EMU): $(EMU_OBJ)
	@echo "Linking $@..."
	@mkdir -p $(dir $@)
	@$(CC_COMMAND) $(CC_LIB_FLAGS) $^ -o $@

$(LIB_CASM) $(TEST_CASM): $(BUILD_DIR)/%.s: %.c
	@echo "$@ <- $<"
	@mkdir -p $(dir $@)
	@$(CC65_EXEC) $(CC65_FLAGS) $< -o $@ --create-full-dep $(patsubst %.s,%.d,$@)

$(LIB_COBJ) $(TEST_COBJ): $(BUILD_DIR)/%.o: $(BUILD_DIR)/%.s
	@echo "$@ <- $<"
	@mkdir -p $(dir $@)
	@$(CA65_EXEC) $(CA65_FLAGS) $< -o $@

$(LIB_ASMOBJ) $(TEST_ASMOBJ): $(BUILD_DIR)/%.o: %.s
	@echo "$@ <- $<"
	@mkdir -p $(dir $@)
	@$(CA65_EXEC) $(CA65_FLAGS) $< -o $@ --create-full-dep $(patsubst %.o,%.d,$@)

$(LIB): $(LIB_OBJ)
	@echo "Linking $@..."
	@mkdir -p $(dir $@)
	@cp $(BASELIB) $@
	@$(AR65_EXEC) r $@ $^

$(TESTS): $(BIN_DIR)/%: $(TEST_BUILD_DIR)/%.o $(LIB) $(LD65_CFG)
	@echo "Linking $@"
	@mkdir -p $(dir $@)
	@$(LD65_EXEC) $< --lib $(LIB) -C $(LD65_CFG) -o $@
