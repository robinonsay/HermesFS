CC := gcc
OUT ?= $(abspath ./)/out
TARGET := hermes
CFLAGS := --debug -std=gnu99 -Werror -Wall
LINK_FLAGS :=
LINK_DIRS := -L$(OUT)/lib
LINK_LIBS := -lHermes
INCLUDE_DIR := -I$(OUT)/include
SRCS := $(wildcard src/*.c)
LIBS := $(wildcard src/libs/*)

export CC OUT

.PHONY: clean core libs

all: $(TARGET) core libs

$(TARGET): core libs
	$(CC) $(CFLAGS) $(INCLUDE_DIR) -fPIC -o $(OUT)/$@ $(SRCS) $(LINK_FLAGS) $(LINK_DIRS) $(LINK_LIBS)

core: libs
	cd src/core && $(MAKE)

libs: $(LIBS)

$(LIBS): $(OUT)
	cd $@ && $(MAKE)

$(OUT):
	mkdir $@

clean:
	rm -r $(OUT)
