include prorab.mk

this_name := mordavokne

this_soname := 0

this_out_dir := build

this_cxxflags += -Wall
this_cxxflags += -Werror
this_cxxflags += -funsigned-char # the 'char' type is unsigned
this_cxxflags += -fstrict-aliasing
this_cxxflags += -std=c++14
this_cxxflags += -g

ifeq ($(debug),true)
    this_cxxflags += -DDEBUG
else
    this_cxxflags += -O3
endif

this_srcs += $(call prorab-src-dir, .)

this_cxxflags += -I$(d)../../../src/morda
this_objcflags += -I$(d)../../src/morda -I$(d)../harness/opengl2 -std=c++14

this_ldlibs += -lmorda -lnitki -lopros -lstdc++
this_ldflags += -L$(d)../../../src/morda/build

ifeq ($(os),windows)
    this_ldlibs += -lmingw32 # these should go first, otherwise linker will complain about undefined reference to WinMain
    this_ldlibs += -lglew32 -lopengl32 -lpng -ljpeg -lz -lfreetype -mwindows

else ifeq ($(os),macosx)
    this_ldlibs += -lGLEW -framework OpenGL -framework Cocoa -lpng -ljpeg -lfreetype

    # TODO: remove?
    # this_ldlibs += -lc++
    # this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib

    this_ldflags += -rdynamic

    this_mm_obj := $(d)$(prorab_obj_dir)objcpp/mordavokne/glue/macosx/glue.mm.o

    define this_rules
        $(this_mm_obj): $(d)mordavokne/glue/macosx/glue.mm
$(.RECIPEPREFIX)@echo compile $$<...
$(.RECIPEPREFIX)$(a)mkdir -p $$(dir $$@)
$(.RECIPEPREFIX)$(a)$(this_cc) -ObjC++ -std=c++11 -c -o "$$@" $(this_objcflags) $$<
    endef
    $(eval $(this_rules))
else ifeq ($(os),linux)
    this_cxxflags += -fPIC
    this_ldlibs += -pthread -lX11 -ldl
    this_ldflags += -rdynamic
endif

ifeq ($(ogles2), true)
    this_cxxflags += -DMORDAVOKNE_RENDER_OPENGLES2
    this_cxxflags += -I$(d)../opengles2
    this_ldflags += -L$(d)../opengles2/build
    this_ldlibs += -lmorda-opengles2-ren
    this_ldlibs += `pkg-config --libs egl`

    ifeq ($(raspberrypi),true)
        this_cxxflags += -I/opt/vc/include
        this_ldflags += -L/opt/vc/lib
    endif
else
    this_cxxflags += -DMORDAVOKNE_RENDER_OPENGL2
    this_cxxflags += -I$(d)../opengl2
    this_ldflags += -L$(d)../opengl2/build
    this_ldlibs += -lmorda-opengl2-ren
endif

this_no_install := true

$(eval $(prorab-build-lib))

# add dependency on libs
$(prorab_this_name): $(abspath $(d)../../../src/morda/build/libmorda$(dot_so))

ifeq ($(ogles2), true)
    $(prorab_this_name): $(abspath $(d)../opengles2/build/libmorda-opengles2-ren$(dot_so))
else
    $(prorab_this_name): $(abspath $(d)../opengl2/build/libmorda-opengl2-ren$(dot_so))
endif


$(eval $(call prorab-include, ../../../src/morda/makefile))

ifeq ($(ogles2), true)
    $(eval $(call prorab-include, ../opengles2/makefile))
else
    $(eval $(call prorab-include, ../opengl2/makefile))
endif
