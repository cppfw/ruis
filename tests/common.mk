include $(d)../harness/modules/module_cfg.mk

this_name := tests

this_srcs += $(call prorab-src-dir, src)

$(eval $(call prorab-config, ../../config))

this_cxxflags += -isystem ../../src -isystem ../harness/modules/ruisapp/src

this__libruis_dir := ../../src/out/$(c)/
this__libruis := $(this__libruis_dir)libruis$(dot_so)

ifeq ($(os),windows)
    this_ldlibs += -lmingw32 -mwindows # these should go first, otherwise linker will complain about undefined reference to WinMain
    this_ldlibs += -lglew32 -lopengl32 -lz -lfreetype
else ifeq ($(os),macosx)
    this_ldlibs += -framework OpenGL -framework Cocoa
    this_ldlibs += $(shell pkg-config --libs freetype2)
    this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib

    this_ldflags += -rdynamic
else ifeq ($(os),linux)
    this_ldflags += -rdynamic
endif

ifeq ($(this__is_interactive),true)
    ifeq ($(ren),gles2)
        this__render := opengles
    else ifeq ($(filter-out gl2,$(ren)),)
        this__render := opengl
    else
        $(error unknown value of 'ren': $(ren))
    endif
else
    this__render := null
endif

ifeq ($(os), linux)
    this__libruisapp := ruisapp-$(this__render)-xorg
    this__libruisapp_subcfg := $(this__render)-xorg
else
    this__libruisapp := ruisapp-$(this__render)
    this__libruisapp_subcfg := $(this__render)
endif

this__libruis_render := ruis-render-$(this__render)

this__libruisapp := ../harness/modules/ruisapp/src/out/$(module_cfg)/$(this__libruisapp_subcfg)/lib$(this__libruisapp)$(dot_so)
this__libruis_render := ../harness/modules/$(this__libruis_render)/src/out/$(module_cfg)/lib$(this__libruis_render)$(dot_so)

ifeq ($(this__is_interactive),true)
    this_ldlibs += $(this__libruisapp)
    this_ldlibs += $(this__libruis_render)
endif

this_ldlibs += $(this__libruis) -lpapki -ltml -lutki -lm

this_no_install := true

$(eval $(prorab-build-app))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(this__libruis_dir) $(dir $(this__libruis_render))

ifeq ($(this__is_interactive),true)
    this_run_name := $(notdir $(abspath $(d)))
    this_test_ld_path += $(dir $(this__libruisapp))
    $(eval $(prorab-run))
else
    $(eval $(prorab-test))
endif

$(eval $(call prorab-include, ../harness/makefile))
