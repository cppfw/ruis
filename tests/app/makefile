include prorab.mk

include $(d)../../raspberrypi.mk

this_name := tests


this_srcs += $(call prorab-src-dir,src)
this_srcs := $(filter-out %glue.cpp, $(this_srcs))


this_cxxflags := -Wall
this_cxxflags += -Wno-comment #no warnings on nested comments
this_cxxflags += -Wno-format
this_cxxflags += -DDEBUG
this_cxxflags += -fstrict-aliasing #strict aliasing!!!
this_cxxflags += -g
this_cxxflags += -O3
this_cxxflags += -std=c++11



ifeq ($(debug), true)
    this_cxxflags += -DDEBUG
endif

this_cxxflags += -I$(d)../../src
this_objcflags += -I$(d)../../src

ifeq ($(ogles2), true)
    this_cxxflags += -DM_RENDER_OPENGLES2

    ifeq ($(raspberrypi),true)
        this_cxxflags += -I/opt/vc/include
        this_ldflags += -L/opt/vc/lib
    endif
endif


ifeq ($(os),windows)
    this_srcs += src/mordavokne/glue/glue.cpp

    this_ldlibs += -lmingw32 #these should go first, otherwise linker will complain about undefined reference to WinMain
    this_ldflags += -L/usr/lib -L/usr/local/lib
    this_ldlibs +=  -lglew32 -lopengl32 -lpng -ljpeg -lz -lfreetype -mwindows

    this_cxxflags += -I/usr/include -I/usr/local/include

    #WORKAROUND for MinGW bug:
    this_cxxflags += -D__STDC_FORMAT_MACROS
else ifeq ($(os),macosx)
    this_ldlibs += -lGLEW -framework OpenGL -framework Cocoa -lpng -ljpeg -lfreetype

    this_mm_obj := $(d)$(prorab_obj_dir)objcpp/src/mordavokne/glue/macosx/glue.o

    define this_rules
        $(this_mm_obj): $(d)src/mordavokne/glue/macosx/glue.mm
		@echo Compiling $$<...
		$(prorab_echo)mkdir -p $$(dir $$@)
		$(prorab_echo)$(CC) -ObjC++ -std=c++11 -c -o "$$@" $(this_objcflags) $$<
    endef
    $(eval $(this_rules))
else ifeq ($(os),linux)
    this_srcs += src/mordavokne/glue/glue.cpp
    this_ldlibs += -pthread -lX11 -ldl
endif

this_ldlibs += $(d)../../src/libmorda$(soext)


this_ldlibs += -lnitki -lpogodi -lstob -lpapki -lstdc++ -lm


ifeq ($(ogles2), true)
    this_ldlibs += `pkg-config --libs egl glesv2`
    this_ldlibs += $(d)../../renderers/opengles2/libmorda-opengles2-ren$(soext)
else
    this_ldlibs += $(d)../../renderers/opengl2/libmorda-opengl2-ren$(soext)
    this_ldlibs += -lGLEW

    ifeq ($(os), macosx)
    else
        this_ldlibs += -lGL
    endif

endif

this_ldflags += -rdynamic

$(eval $(prorab-build-app))

ifeq ($(prorab_os), macosx)
    $(prorab_this_staticlib): $(this_mm_obj)

    $(prorab_this_name): $(this_mm_obj)
endif



define this_rules
test:: $(prorab_this_name)
	@echo running $$^...
	@(cd $(d); LD_LIBRARY_PATH=../../src:../../renderers/opengl2 $$^)
endef
$(eval $(this_rules))


#add dependency on libmorda
ifeq ($(prorab_os),windows)
    $(d)libmorda$(soext): $(abspath $(d)../../src/libmorda$(soext))
	@cp $< $@

    $(prorab_this_name): $(d)libmorda$(soext)

    define this_rules
        clean::
		@rm -f $(d)libmorda$(soext)
    endef
    $(eval $(this_rules))
else
    $(prorab_this_name): $(abspath $(d)../../src/libmorda$(soext))
endif

#add dependency on opengl2 renderer
$(prorab_this_name): $(abspath $(d)../../renderers/opengl2/libmorda-opengl2-ren$(soext))

$(eval $(call prorab-include,$(d)../../src/makefile))
$(eval $(call prorab-include,$(d)../../renderers/opengl2/makefile))
