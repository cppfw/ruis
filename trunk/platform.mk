platform := linux
operating_system := $(shell uname)
ifeq ($(patsubst MINGW%,MINGW,$(operating_system)), MINGW)
    platform := windows
endif
ifeq ($(operating_system), Darwin)
    platform := macosx
endif
