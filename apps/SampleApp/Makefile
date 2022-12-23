CC ?= gcc
OUT ?= $(abspath ./)/out
APP_OUT ?= $(OUT)/app
HERMES_PATH ?= $(OUT)
NAME := SampleApp
CFLAGS := --debug -std=c99 -pedantic -Werror -Wall
LINK_FLAGS :=
LINK_DIRS := -L$(HERMES_PATH)/lib
LINK_LIBS := -lHermes
INCLUDE := -I$(HERMES_PATH)/include
TARGET := lib$(NAME).so
SRCS := $(wildcard src/*.c)

.PHONY: clean

all: $(TARGET)

$(TARGET): $(APP_OUT)
	$(CC) -shared $(CFLAGS) $(INCLUDE) -fPIC -o $(APP_OUT)/$@ $(SRCS) $(LINK_FLAGS) $(LINK_DIRS) $(LINK_LIBS)

$(APP_OUT):
	mkdir $@

clean:
	rm $(OUT)/$(TARGET)

