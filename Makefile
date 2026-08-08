.PHONY: all production release tests clean

CC := clang
BIN := sea64

STD := -std=c2x
WARN := -Wall -Wextra -Wpedantic

INCLUDES := $(addprefix -I,$(shell find include tests -type d))
LIBS := `pkg-config --cflags --libs sdl3`

# Find all application sources and filter out main.c
ALL_SRCS := $(shell find src -name '*.c')
APP_SRCS := $(filter-out src/main.c, $(ALL_SRCS))
APP_OBJS := $(APP_SRCS:.c=.o)
MAIN_OBJ := src/main.o

# Find all test source files and map them to separate test executables
TEST_SRCS := $(shell find tests -name '*.c')
TEST_BINS := $(TEST_SRCS:.c=)

# Default target
all: production

# Production build (debug + sanitizers)
production: CFLAGS := -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer $(STD) $(WARN) $(INCLUDES)
production: LDFLAGS := -ljansson -fsanitize=address,undefined $(LIBS)
production: $(BIN)

# Release build (fast)
release: CFLAGS := -O3 -march=native -flto -fno-plt -fomit-frame-pointer -DNDEBUG $(STD) $(WARN) $(INCLUDES) 
release: LDFLAGS := -flto $(LIBS)
release: $(BIN)

# Tests build - sets flags and builds ALL test binaries
tests: CFLAGS := -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer $(STD) $(WARN) $(INCLUDES)
tests: LDFLAGS := -ljansson -fsanitize=address,undefined $(LIBS)
tests: $(TEST_BINS)

# Main Binary Link Rule
$(BIN): $(APP_OBJS) $(MAIN_OBJ)
	@echo "[linking main] $(BIN)"
	@$(CC) -o $@ $^ $(LDFLAGS)

# Rule to build ANY test binary (e.g. tests/pla_test or tests/6510_test)
# Links core application objects + the single test source file
tests/%: tests/%.c $(APP_OBJS)
	@echo "[building test] $@"
	@$(CC) $(CFLAGS) $< $(APP_OBJS) -o $@ $(LDFLAGS)

# Clean
clean:
	@echo "[cleaned]"
	@rm -rf $(BIN) $(TEST_BINS) *.gch ncore.* 2>/dev/null || true
	@find src tests -name '*.o' -type f -delete 2>/dev/null || true
