platform := linux

operating_system := $(shell uname -o)
ifeq ($(operating_system), Msys)
    platform := win32
endif

binary_name := tests

ifeq ($(platform),win32)
    binary_name := $(binary_name).exe
endif

includes := -I../../src

ifeq ($(platform),win32)
    libs := -L/usr/lib -L/usr/local/lib -lting ../../src/libmorda.a -lglew32 -lopengl32 -lstob -lpng -ljpeg -lz
    includes += -I"../../../ting/src" -I/usr/include
endif
ifeq ($(platform),darwin)
    libs := 
    includes +=
endif
ifeq ($(platform),linux)
    libs := -lting ../../src/libmorda.so -lGLEW -lstob
    includes += -I../../ext
endif

#external compiler flags (to be passed to make from command line)
ecf :=

#compiler flags
cf := $(ecf)
cf += -Wall
cf += -Wno-comment #no warnings on nested comments
cf += -Wno-format
cf += -DDEBUG
cf += -fstrict-aliasing #strict aliasing!!!

default: $(binary_name)

test: $(binary_name)
	@echo running $^...
	@LD_LIBRARY_PATH=../../src ./$^

$(binary_name): src/main.cpp
	@echo Building $@...
	@g++ -o $@ $(cf) $^ $(includes) $(libs)



clean:
	@rm -rf $(binary_name)

include ../../targets.mk
