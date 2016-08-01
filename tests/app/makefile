include prorab.mk

this_name := tests


this_srcs += src/main.cpp
this_srcs += src/mordavokne/App.cpp


this_cxxflags := -Wall
this_cxxflags += -Wno-comment #no warnings on nested comments
this_cxxflags += -Wno-format
this_cxxflags += -DDEBUG
this_cxxflags += -fstrict-aliasing #strict aliasing!!!
this_cxxflags += -g
this_cxxflags += -O3
this_cxxflags += -std=c++11

this_cxxflags += -D_GLIBCXX_USE_CXX11_ABI=0 #use old ABI

ifeq ($(debug), true)
    this_cxxflags += -DDEBUG
endif

ifeq ($(prorab_os),windows)
    this_srcs += src/mordavokne/glue/glue.cpp

    this_ldlibs += -lmingw32 #these should go first, otherwise linker will complain about undefined reference to WinMain
    this_ldlibs += $(prorab_this_dir)../../src/libmorda$(prorab_lib_extension)
    this_ldflags += -L/usr/lib -L/usr/local/lib
    this_ldlibs +=  -lglew32 -lopengl32 -lpng -ljpeg -lz -lfreetype -mwindows

    this_cxxflags += -I/usr/include -I/usr/local/include

    #WORKAROUND for MinGW bug:
    this_cxxflags += -D__STDC_FORMAT_MACROS
else ifeq ($(prorab_os),macosx)
    this_mm_src := mordavokne/glue/macosx/glue.mm
    this_ldlibs += $(prorab_this_dir)../../src/libmorda$(prorab_lib_extension) -lGLEW -framework OpenGL -framework Cocoa -lpng -ljpeg -lfreetype

    this_mm_obj := $(prorab_this_dir)$(prorab_obj_dir)mordavokne/glue/macosx/glue.o

    define this_rules
        $(this_mm_obj): $(prorab_this_dir)mordavokne/glue/macosx/glue.mm
		@echo Compiling $$<...
		$(prorab_echo)mkdir -p $$(dir $$@)
		$(prorab_echo)$(CC) -ObjC++ -std=c++11 -c -o "$$@" $(this_objcflags) $$<
    endef
    $(eval $(this_rules))
else ifeq ($(prorab_os),linux)
    this_srcs += src/mordavokne/glue/glue.cpp
    this_ldlibs += $(prorab_this_dir)../../src/libmorda$(prorab_lib_extension) -lGLEW -pthread -lGL -lX11 -ldl
endif

this_ldlibs += -lnitki -laika -lpogodi -lstob -lpapki

this_ldflags += -rdynamic

$(eval $(prorab-build-app))

ifeq ($(prorab_os), macosx)
    $(prorab_this_staticlib): $(this_mm_obj)

    $(prorab_this_name): $(this_mm_obj)
endif



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
