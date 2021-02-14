include prorab.mk

this_name := tests

this_srcs += $(call prorab-src-dir,.)

$(eval $(call prorab-config, ../../config))

this_cxxflags += -I$(d)../../src/morda
this_objcflags += -I$(d)../../src/morda

ifeq ($(ogles2), true)
    this_cxxflags += -DM_RENDER_OPENGLES2

    ifeq ($(raspberrypi),true)
        this_cxxflags += -I/opt/vc/include
        this_ldflags += -L/opt/vc/lib
    endif
endif

ifeq ($(os),windows)
    this_ldlibs += -lmingw32 # these should go first, otherwise linker will complain about undefined reference to WinMain
    this_ldlibs +=  -lglew32 -lopengl32 -lpng -ljpeg -lz -lfreetype -mwindows
else ifeq ($(os),macosx)
    this_ldlibs += -lGLEW -framework OpenGL -framework Cocoa -lpng -ljpeg -lfreetype

    this_ldflags += -rdynamic

    this_mm_obj := $(d)$(prorab_obj_dir)objcpp/src/mordavokne/glue/macosx/glue.o

    define this_rules
        $(this_mm_obj): $(d)src/mordavokne/glue/macosx/glue.mm
		@echo Compiling $$<...
		$(a)mkdir -p $$(dir $$@)
		$(a)$(this_cc) -ObjC++ -std=c++11 -c -o "$$@" $(this_objcflags) $$<
    endef
    $(eval $(this_rules))
else ifeq ($(os),linux)
    this_cxxflags += -fPIC
    this_ldlibs += -pthread -ldl
    this_ldflags += -rdynamic
endif

this_ldlibs += $(d)../../src/morda/out/$(c)/libmorda$(dot_so)

this_ldlibs += -lnitki -lopros -lpuu -lpapki -lstdc++ -lm

this_no_install := true

$(eval $(prorab-build-app))

this_dirs := $(subst /, ,$(d))
this_test := $(word $(words $(this_dirs)),$(this_dirs))

define this_rules
test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(this_test)
$(.RECIPEPREFIX)$(a)(cd $(d); LD_LIBRARY_PATH=../../src/morda/out/$(c) $$^)
$(.RECIPEPREFIX)@myci-passed.sh
endef
$(eval $(this_rules))

# add dependency on libmorda
ifeq ($(os),windows)
    $(d)libmorda$(dot_so): $(abspath $(d)../../src/morda/out/$(c)/libmorda$(dot_so))
	@cp $< $@

    $(prorab_this_name): $(d)libmorda$(dot_so)

    define this_rules
        clean::
		@rm -f $(d)libmorda$(dot_so)
    endef
    $(eval $(this_rules))
else
    $(prorab_this_name): $(abspath $(d)../../src/morda/out/$(c)/libmorda$(dot_so))
endif

$(eval $(call prorab-include, ../../src/morda/makefile))
