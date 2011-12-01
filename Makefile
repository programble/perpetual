# Output executable name
OUTPUT := perpetual

# Target to build (debug or release)
TARGET := debug

# Libraries to use
LIBS   := talloc

# Compiler options
CC                := clang
override CFLAGS   += -std=c99 -D_BSD_SOURCE -MMD $(shell pkg-config --cflags-only-other $(LIBS))
override INCLUDES += -Iinclude/ $(shell pkg-config --cflags-only-I $(LIBS))
override WARNINGS += -Wall -Wextra
override LDFLAGS  += $(shell pkg-config --libs $(LIBS))

# Target-specific flags
ifeq ($(TARGET),debug)
	override CFLAGS += -ggdb -O0 -DDEBUG
endif

ifeq ($(TARGET),release)
	override CFLAGS += -DNDEBUG
endif

# Find all sources
SOURCES := $(shell find src/ -name '*.c')
OBJECTS := $(SOURCES:%.c=%.o)
DFILES  := $(SOURCES:%.c=%.d)

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	@echo -e " [\033[32;1mLD\033[0m] $@"
	@$(CC) $(LDFLAGS) $^ -o $@

# Dependency files from -MMD
-include $(DFILES)

%.o: %.c
	@echo -e " [\033[34;1mCC\033[0m] $@"
	@$(CC) $(CFLAGS) $(INCLUDES) $(WARNINGS) -c $< -o $@

clean:
	@echo -e " [\033[31;1mRM\033[0m] $(OBJECTS)"
	@rm -f $(OBJECTS)
	@echo -e " [\033[31;1mRM\033[0m] $(DFILES)"
	@rm -f $(DFILES)
	@echo -e " [\033[31;1mRM\033[0m] $(OUTPUT)"
	@rm -f $(OUTPUT)

flags:
	@echo "CFLAGS:   $(CFLAGS)"
	@echo "INCLUDES: $(INCLUDES)"
	@echo "WARNINGS: $(WARNINGS)"
	@echo "LDFLAGS:  $(LDFLAGS)"

.PHONY: all clean flags
