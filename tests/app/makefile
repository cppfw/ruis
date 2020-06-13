include prorab.mk

include $(d)../../raspberrypi.mk

ifneq ($(raspberrypi),true)

    this_name := tests

    this_out_dir := build

    this_srcs += $(call prorab-src-dir, src)

    this_cxxflags += -Wall
    this_cxxflags += -Wno-comment # no warnings on nested comments
    this_cxxflags += -Wno-format # no warnings about format
    this_cxxflags += -Wno-format-security # no warnings about format
    this_cxxflags += -DDEBUG
    this_cxxflags += -fstrict-aliasing
    this_cxxflags += -g
    this_cxxflags += -std=c++14

    ifeq ($(debug), true)
        this_cxxflags += -DDEBUG
    else
        this_cxxflags += -O3
    endif

    this_cxxflags += -I$(d)../../src/morda -I$(d)../harness/mordavokne

    this_ldflags += -L$(d)../../src/morda/build
    # this_ldlibs += $(d)../../src/morda/build/libmorda$(dot_so)
    this_ldlibs += $(d)../harness/mordavokne/build/libmordavokne$(dot_so)

    # this_ldlibs +=
    this_ldlibs += -lmorda -lpapki -lpuu -lunikod -lstdc++ -lm

    ifeq ($(os),windows)
        this_ldlibs += -lmingw32 # these should go first, otherwise linker will complain about undefined reference to WinMain
        this_ldlibs += -lglew32 -lopengl32 -lpng -ljpeg -lz -lfreetype -mwindows

    else ifeq ($(os),macosx)
        this_ldlibs += -lGLEW -framework OpenGL -framework Cocoa -lpng -ljpeg -lfreetype

        this_ldflags += -rdynamic

        this_mm_obj := $(d)$(prorab_obj_dir)objcpp/mordavokne/glue/macosx/glue.mm.o

        define this_rules
            $(this_mm_obj): $(d)mordavokne/glue/macosx/glue.mm
            @echo Compiling $$<...
            $(a)mkdir -p $$(dir $$@)
            $(a)$(this_cc) -ObjC++ -std=c++11 -c -o "$$@" $(this_objcflags) $$<
        endef
        $(eval $(this_rules))
    else ifeq ($(os),linux)
        this_cxxflags += -fPIC
        # this_ldlibs += -pthread -lX11 -ldl
        this_ldflags += -rdynamic
    endif

    ifeq ($(ogles2), true)
        this_ldlibs += `pkg-config --libs egl glesv2`
    else
        ifeq ($(os), macosx)
        else ifeq ($(os), windows)
        else
            this_ldlibs += -lGL -lGLEW
        endif

    endif

    this_no_install := true

    $(eval $(prorab-build-app))

    define this_rules
        run:: $(prorab_this_name)
$(.RECIPEPREFIX)@echo running $$^...
$(.RECIPEPREFIX)$(a)(cd $(d); LD_LIBRARY_PATH=../../src/morda/build:../harness/mordavokne/build $$^)
    endef
    $(eval $(this_rules))

    # add dependency on libmordavokne
    $(prorab_this_name): $(abspath $(d)../harness/mordavokne/build/libmordavokne$(dot_so))

    # add dependency on libmorda
    ifeq ($(os),windows)
        $(prorab_this_name): $(d)libmorda$(dot_so)

        define this_rules
            $(d)libmorda$(dot_so): $(abspath $(d)../../src/morda/build/libmorda$(dot_so))
$(.RECIPEPREFIX)$(a)cp $< $@

            clean::
$(.RECIPEPREFIX)$(a)rm -f $(d)libmorda$(dot_so)
        endef
        $(eval $(this_rules))
    else
        $(prorab_this_name): $(abspath $(d)../../src/morda/build/libmorda$(dot_so))
    endif

    $(eval $(call prorab-include, ../../src/morda/makefile))
    $(eval $(call prorab-include, ../harness/mordavokne/makefile))

endif
