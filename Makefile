# Include output from ./configure
include config.mk

# Find sources
SOURCES := $(shell find src/ -name '*.c')
OBJECTS := $(SOURCES:%.c=%.o)
DFILES  := $(SOURCES:%.c=%.d)

all: config.mk $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	@echo -e " [\033[32;1mLD\033[0m] $@"
	@$(CC) $(LDFLAGS) $^ -o $@

# Dependency files from -MMD
-include $(DFILES)

%.o: %.c
	@echo -e " [\033[34;1mCC\033[0m] $@"
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@echo -e " [\033[31;1mRM\033[0m] $(OBJECTS)"
	@rm -f $(OBJECTS)
	@echo -e " [\033[31;1mRM\033[0m] $(DFILES)"
	@rm -f $(DFILES)
	@echo -e " [\033[31;1mRM\033[0m] $(OUTPUT)"
	@rm -f $(OUTPUT)

config.mk:
	@echo "Run ./configure before running make"
	@false

install: $(OUTPUT)
	@echo -e " [\033[33;1mIN\033[0m] $(OUTPUT)"
	@install -D $(OUTPUT) $(DESTDIR)$(PREFIX)/bin/$(OUTPUT)

.PHONY: all clean
