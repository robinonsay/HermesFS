CC ?= gcc
OUT ?= $(abspath ./)/out
LIB_OUT ?= $(OUT)/lib
INC_OUT ?= $(OUT)/include
NAME := Hermes
CFLAGS := --debug -std=gnu99 -pedantic -Werror -Wall
LINK_FLAGS :=
LINK_DIRS ?= -L$(LIB_OUT)
LINK_LIBS := -lHermesUtils
TARGET := lib$(NAME).so
SRCS := $(wildcard */*.c)
INCLUDE := $(wildcard */*.h) $(wildcard *.h)
INCLUDE_DIR = -I$(INC_OUT)

.PHONY: clean

all: $(TARGET)

$(TARGET): $(LIB_OUT) $(INCLUDE)
	$(CC) -shared $(CFLAGS) $(INCLUDE_DIR) -fPIC -o $(LIB_OUT)/$@ $(SRCS) $(LINK_FLAGS) $(LINK_DIRS) $(LINK_LIBS)

$(INCLUDE): $(INC_OUT)
	cp $@ $^

$(LIB_OUT) $(INC_OUT): $(OUT)
	mkdir $@

$(OUT):
	mkdir $@


clean:
	rm $(OUT)/$(TARGET)
	rm -r $(INC_OUT)

