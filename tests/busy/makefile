include prorab.mk

# include $(d)../../raspberrypi.mk

# ifneq ($(raspberrypi),true)

    this_name := tests

    this_out_dir := build

    this_srcs += $(call prorab-src-dir, src)

    this_cxxflags += -Wall
    this_cxxflags += -Werror
    this_cxxflags += -Wno-comment # no warnings on nested comments
    this_cxxflags += -Wno-format # no warnings about format
    this_cxxflags += -Wno-format-security # no warnings about format
    this_cxxflags += -DDEBUG
    this_cxxflags += -fstrict-aliasing
    this_cxxflags += -g
    this_cxxflags += -std=c++17

    ifeq ($(debug), true)
        this_cxxflags += -DDEBUG
    else
        this_cxxflags += -O3
    endif

    this_cxxflags += -I$(d)../../src/morda -I$(d)../harness/mordavokne

    this_ldflags += -L$(d)../../src/morda/build
    this_ldflags += -L$(d)../harness/mordavokne/build

    ifeq ($(os),windows)
        this_ldlibs += -lmingw32 # these should go first, otherwise linker will complain about undefined reference to WinMain
        this_ldlibs += -lglew32 -lopengl32 -lpng -ljpeg -lz -lfreetype -mwindows
        this_ldlibs += -lstdc++
    else ifeq ($(os),macosx)
        this_ldlibs += -framework OpenGL -framework Cocoa -lpng -ljpeg -lfreetype
        this_ldlibs += -lc++
        this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib

        this_ldflags += -rdynamic
    else ifeq ($(os),linux)
        this_cxxflags += -fPIC
        this_ldlibs += -lstdc++
        this_ldflags += -rdynamic
    endif

    this_ldlibs += -lmordavokne -lmorda -lpapki -lpuu -lutki -lm

    ifeq ($(ogles2), true)
        this_ldflags += -L$(d)../harness/opengles2/build
        this_ldlibs += -lmorda-opengles2-ren
    else
        this_ldflags += -L$(d)../harness/opengl2/build
        this_ldlibs += -lmorda-opengl2-ren
        ifeq ($(os), macosx)
        else ifeq ($(os), windows)
        else
            this_ldlibs += -lGL -lGLEW
        endif
    endif

    this_no_install := true

    $(eval $(prorab-build-app))

    define this_rules
        run_busy:: $(prorab_this_name)
$(.RECIPEPREFIX)@echo running $$^...
$(.RECIPEPREFIX)$(a)(cd $(d); LD_LIBRARY_PATH=../../src/morda/build:../harness/mordavokne/build:../harness/opengl2/build:../harness/opengles2/build $$^)
    endef
    $(eval $(this_rules))

    # add dependency on libmordavokne
    $(prorab_this_name): $(abspath $(d)../harness/mordavokne/build/libmordavokne$(dot_so))

    # add dependency on libmorda, libmordavokne
    ifeq ($(os),windows)
        $(prorab_this_name): $(d)libmorda$(dot_so) $(d)libmordavokne$(dot_so)

        define this_rules
            $(d)libmorda$(dot_so): $(abspath $(d)../../src/morda/build/libmorda$(dot_so))
$(.RECIPEPREFIX)@echo "copy $$(notdir $$@)"
$(.RECIPEPREFIX)$(a)cp $$< $$@

            $(d)libmordavokne$(dot_so): $(abspath $(d)../harness/mordavokne/build/libmordavokne$(dot_so))
$(.RECIPEPREFIX)@echo "copy $$(notdir $$@)"
$(.RECIPEPREFIX)$(a)cp $$< $$@

            clean::
$(.RECIPEPREFIX)$(a)rm -f $(d)libmorda$(dot_so)
        endef
        $(eval $(this_rules))
    else
        $(prorab_this_name): $(abspath $(d)../../src/morda/build/libmorda$(dot_so)) $(abspath $(d)../harness/mordavokne/build/libmordavokne$(dot_so))
    endif

    $(eval $(call prorab-include, ../../src/morda/makefile))
    $(eval $(call prorab-include, ../harness/mordavokne/makefile))

# endif
