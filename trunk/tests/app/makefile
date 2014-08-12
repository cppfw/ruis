include prorab.mk

this_name := tests


this_srcs := src/main.cpp


this_cflags := -Wall
this_cflags += -Wno-comment #no warnings on nested comments
this_cflags += -Wno-format
this_cflags += -DDEBUG
this_cflags += -fstrict-aliasing #strict aliasing!!!
this_cflags += -g
this_cflags += -std=c++11

ifeq ($(debug), true)
    this_cflags += -DDEBUG
endif

ifeq ($(prorab_os),windows)
    this_ldlibs += -lmingw32 #these should go first, otherwise linker will complain about undefined reference to WinMain
    this_ldlibs += $(prorab_this_dir)../../src/libmorda.dll
    this_ldflags += -L/usr/lib -L/usr/local/lib
    this_ldlibs +=  -lglew32 -lopengl32 -lting -lstob -lpng -ljpeg -lz -lfreetype -mwindows

    this_cflags += -I/usr/include -I/usr/local/include
else ifeq ($(prorab_os),macosx)
    this_ldlibs += $(prorab_this_dir)../../src/libmorda.a -lGLEW -framework OpenGL -framework Cocoa -lstob -lting -lpng -ljpeg -lfreetype
else ifeq ($(prorab_os),linux)
    this_ldlibs += -lting $(prorab_this_dir)../../src/libmorda.so -lGLEW -lstob -pthread -lGL
endif



$(eval $(prorab-build-app))


define this_rules
test:: $(prorab_this_name)
	@echo running $$^...
	@(cd $(prorab_this_dir); LD_LIBRARY_PATH=../../src $$^)
endef
$(eval $(this_rules))


#add dependency on libmorda
ifeq ($(prorab_os),windows)
    $(prorab_this_dir)libmorda$(prorab_lib_extension): $(abspath $(prorab_this_dir)../../src/libmorda$(prorab_lib_extension))
	@cp $< $@

    $(prorab_this_name): $(prorab_this_dir)libmorda$(prorab_lib_extension)

    define this_rules
        clean::
		@rm -f $(prorab_this_dir)libmorda$(prorab_lib_extension)
    endef
    $(eval $(this_rules))
else
    $(prorab_this_name): $(abspath $(prorab_this_dir)../../src/libmorda$(prorab_lib_extension))
endif


$(eval $(call prorab-include,$(prorab_this_dir)../../src/makefile))
