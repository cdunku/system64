.PHONY: all production release clean

CC := gcc
BIN := sea64

STD := -std=c2x
WARN := -Wall -Wextra -Wpedantic

INCLUDES := $(addprefix -I,$(shell find include -type d))

LIBS := `pkg-config --cflags --libs sdl3`

# Recursively grab all source files
SRCS := $(shell find src -name '*.c')
OBJS := $(SRCS:.c=.o)

# Default target
all: production

# Production build (debug + sanitizers)
production: CFLAGS := -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer $(STD) $(WARN) $(INCLUDES)
production: LDFLAGS := -ljansson -fsanitize=address,undefined $(LIBS)
production: $(BIN)

# Release build (fast)
release: CFLAGS := -O3 -march=native -flto -fno-plt -fomit-frame-pointer -DNDEBUG $(STD) $(WARN) $(INCLUDES) 
release: LDFLAGS := -ljansson -flto $(LIBS)
release: $(BIN)

# Link
$(BIN): $(OBJS)
	@echo "[linking]   $(OBJS)"
	@echo "[CFLAGS]    $(CFLAGS)"
	@echo "[LDFLAGS]   $(LDFLAGS)"
	@$(CC) -o $@ $(OBJS) $(LDFLAGS)
	@echo "[produced]  $(BIN)"

# Compile
%.o: %.c 
	@echo "[compiling] $<"
	@echo "[CFLAGS]    $(CFLAGS)"
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	@echo "[cleaned]"
	@rm -rf $(OBJS) $(BIN) *.gch ncore.* 2>/dev/null || true
