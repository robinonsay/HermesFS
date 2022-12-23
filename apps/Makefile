CC := gcc
OUT ?= $(abspath ./)/out
APP_OUT ?= $(OUT)/app
HERMES_PATH ?= $(OUT)
APPS := SampleApp

.PHONY: clean

export APP_OUT HERMES_PATH

all: $(APPS)

$(APPS): $(APP_OUT)
	cd $@ && $(MAKE)

$(APP_OUT):
	mkdir $@

clean:
	rm $(OUT)/$(TARGET)

