include prorab.mk

define mordavokne_rules

    this_name := mordavokne-$1

    this_soname := 0

    $$(eval $$(call prorab-config, ../../../config))

    this_srcs += $$(call prorab-src-dir, .)

    this_cxxflags += -I$(d)../../../src/morda

    this_ldlibs += -lmorda -lnitki -lopros -lpapki
    this_ldflags += -L$(d)../../../src/morda/out/$(c)

    ifeq ($(os),windows)
        this_ldlibs += -lmingw32 # these should go first, otherwise linker will complain about undefined reference to WinMain
        this_ldlibs += -lglew32 -lopengl32 -lpng -ljpeg -lz -lfreetype -mwindows
    else ifeq ($(os),macosx)
        this_ldlibs += -lGLEW -framework OpenGL -framework Cocoa -lpng -ljpeg -lfreetype

        this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib

        this_objcflags += -I$(d)../../../src/morda -I$(d)../$1

        this_ldflags += -rdynamic

        this_mm_obj := $$(d)$$(this_out_dir)$(prorab_obj_dir)objcpp/mordavokne/glue/macosx/glue.mm.o

        define this_rules
            $$(this_mm_obj): $(d)mordavokne/glue/macosx/glue.mm
$(.RECIPEPREFIX)@echo compile $$$$<...
$(.RECIPEPREFIX)$(a)mkdir -p $$$$(dir $$$$@)
$(.RECIPEPREFIX)$(a)$$(this_cc) -ObjC++ -std=c++11 -c -o "$$$$@" $$(this_objcflags) $$$$<
        endef
        $$(eval $$(this_rules))
    else ifeq ($(os),linux)
        this_ldlibs += -pthread -lX11 -ldl
        this_ldflags += -rdynamic
    endif

    this_cxxflags += -DMORDAVOKNE_RENDER_$(shell echo $1 | tr '[:lower:]' '[:upper:]')
    this_ldlibs += -lmorda-$1-ren
    this_cxxflags += -I$(d)../$1
    this_ldflags += -L$(d)../$1/out/$(c)

    ifeq ($1,opengles2)
        this_raspberrypi := $(shell test -d /opt/vc && echo true)
        ifeq ($(this_raspberrypi), true)
            this_cxxflags += -DM_RASPBERRYPI -I/opt/vc/include
            this_ldflags += -L/opt/vc/lib
            this_ldlibs += -lbrcmEGL `pkg-config --libs glesv2`
        else
            this_ldlibs += -lEGL
        endif
    endif

    this_no_install := true

    $$(eval $$(prorab-build-lib))

    ifeq ($(os),macosx)
        $$(prorab_this_static_lib): $$(this_mm_obj)

        $$(prorab_this_name): $$(this_mm_obj)
    endif

    # add dependency on libs
    $$(prorab_this_name): $$(abspath $$(d)../../../src/morda/out/$$(c)/libmorda$$(dot_so))

    $$(prorab_this_name): $$(abspath $$(d)../$1/out/$$(c)/libmorda-$1-ren$$(dot_so))

    $$(eval $$(call prorab-include, ../opengl2/makefile))

    $$(eval $$(prorab-clear-this-vars))
endef

$(eval $(call mordavokne_rules,opengl2))

ifeq ($(os), linux)
    $(eval $(call mordavokne_rules,opengles2))
endif

# clear variable
mordavokne_rules :=

$(eval $(call prorab-include, ../../../src/morda/makefile))
