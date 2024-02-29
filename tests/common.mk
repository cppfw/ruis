include $(d)../harness/modules/module_cfg.mk

this_name := tests

this_srcs += $(call prorab-src-dir, src)

$(eval $(call prorab-config, ../../config))

this_cxxflags += -isystem ../../src -isystem ../harness/modules/ruisapp/src

this_libruis_dir := ../../src/out/$(c)/
this_libruis := $(this_libruis_dir)libruis$(dot_so)

ifeq ($(os),windows)
    this_ldlibs += -lmingw32 -mwindows # these should go first, otherwise linker will complain about undefined reference to WinMain
    this_ldlibs += -lglew32 -lopengl32 -lz -lfreetype
else ifeq ($(os),macosx)
    this_ldlibs += -framework OpenGL -framework Cocoa -lfreetype
    this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib

    this_ldflags += -rdynamic
else ifeq ($(os),linux)
    this_ldflags += -rdynamic
endif

ifeq ($(ren),gles2)
    this_render := opengles
else ifeq ($(filter-out gl2,$(ren)),)
    this_render := opengl
else
    $(error unknown value of 'ren': $(ren))
endif

this_libruisapp := ruisapp-$(this_render)
this_libruis_render := ruis-render-$(this_render)

this_libruisapp := ../harness/modules/ruisapp/src/out/$(module_cfg)/lib$(this_libruisapp)$(dot_so)
this_libruis_render := ../harness/modules/$(this_libruis_render)/src/out/$(module_cfg)/lib$(this_libruis_render)$(dot_so)

ifeq ($(this_is_interactive),true)
    this_ldlibs += $(this_libruisapp)
    this_ldlibs += $(this_libruis_render)
endif

this_ldlibs += $(this_libruis) -lpapki -ltml -lutki -lm

this_no_install := true

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_libruisapp) $(this_libruis)))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(this_libruis_dir)

ifeq ($(this_is_interactive),true)
    this_run_name := $(notdir $(abspath $(d)))
    this_test_ld_path += $(dir $(this_libruisapp)) $(dir $(this_libruis_render))
    $(eval $(prorab-run))
else
    $(eval $(prorab-test))
endif
$(eval $(this_rules))

$(eval $(call prorab-include, ../harness/makefile))
