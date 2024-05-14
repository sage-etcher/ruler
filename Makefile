
DESTDIR := /usr/local
BINDIR := $(DESTDIR)/bin

SRC_DIR := .
BUILD_DIR := .

TARGET := ruler
SRCS := $(SRC_DIR)/main.c
OBJS := $(BUILD_DIR)/main.c.o

LIBS := $(shell pkg-config --libs sdl2)
CFLAGS := $(shell pkg-config --cflags sdl2)


build: $(BUILD_DIR)/$(TARGET)

clean:
	rm -f $(OBJS) $(BUILD_DIR)/$(TARGET)


$(BUILD_DIR)/$(TARGET): $(OBJS)
	mkdir -pv $(dir $@)
	cc -o $@ $(OBJS) $(LIBS)

$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c $(SRCS)
	mkdir -pv $(dir $@)
	cc -c -o $@ $< $(CFLAGS)


install: $(BINDIR)/$(TARGET)

uninstall:
	rm -f $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(BUILD_DIR)/$(TARGET)
	install -d -m 755 $(BINDIR)
	install -m 755 $(BUILD_DIR)/$(TARGET) $(BINDIR)/$(TARGET)

run:
	$(shell $(TARGET))

buildrun:
	$(shell $(BUILD_DIR)/$(TARGET))

test:
	@echo "DESTDIR   = $(DESTDIR)"
	@echo "BINDIR    = $(BINDIR)"
	@echo "SRC_DIR   = $(SRC_DIR)"
	@echo "BUILD_DIR = $(BUILD_DIR)"
	@echo "TARGET    = $(TARGET)"
	@echo "SRCS      = $(SRCS)"
	@echo "OBJS      = $(OBJS)"
	@echo "LIBS      = $(LIBS)"
	@echo "CFLAGS    = $(CFLAGS)"


.PHONY: build install uninstall clean test

# vim: ts=4 sts=4 sw=4 noet fdm=marker
