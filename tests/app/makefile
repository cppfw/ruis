include ../../platform.mk


binary_name := tests

ifeq ($(platform),windows)
    binary_name := $(binary_name).exe
endif

includes := -I../../src

ifeq ($(platform),windows)
    libs := -lmingw32 #these should go first, otherwise linker will complain about undefined reference to WinMain
#    libs := -mwindows #if we do not want console output, e.g. in production version
    libs += ../../src/libmorda_main.a ../../src/libmorda.dll
    libs += -L/usr/lib -L/usr/local/lib
    libs +=  -lglew32 -lopengl32
    includes += -I/usr/include
endif
ifeq ($(platform),macosx)
    libs := ../../src/libmorda.a -lGLEW -framework OpenGL -framework Cocoa -lstob -lting -lpng -ljpeg -lfreetype
    includes +=
endif
ifeq ($(platform),linux)
    libs := -lting ../../src/libmorda.so -lGLEW -lstob -pthread -lGL
    includes += -I../../ext
endif

#external compiler flags (to be passed to make from command line)
ecf :=

#compiler flags
cf := $(ecf)
cf += -Wall
cf += -Wno-comment #no warnings on nested comments
cf += -Wno-format
cf += -std=c++11
cf += -DDEBUG
cf += -fstrict-aliasing #strict aliasing!!!
cf += -g

default: $(binary_name)

.PHONY:
test: $(binary_name)
	@$(MAKE) clean
	@$(MAKE)
	@echo running $^...
ifeq ($(platform), windows)
	@cp ../../src/libmorda.dll .
	./$^
endif
ifeq ($(platform), linux)
	@LD_LIBRARY_PATH=../../src ./$^
endif
ifeq ($(platform), macosx)
	@DYLD_LIBRARY_PATH=../../src ./$^
endif


$(binary_name): src/main.cpp
	@$(MAKE) -C ../../src
	@echo Building $@...
	@g++-4.7 -o $@ $(cf) $^ $(includes) $(libs)


.PHONY:
clean:
	@rm -rf $(binary_name)

include ../../targets.mk
