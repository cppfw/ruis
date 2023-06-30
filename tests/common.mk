include $(d)../harness/modules/module_cfg.mk

this_name := tests

this_srcs += $(call prorab-src-dir, src)

$(eval $(call prorab-config, ../../config))

this_cxxflags += -I../../src -I../harness/modules/mordavokne/src

this_libmorda_dir := ../../src/out/$(c)/
this_libmorda := $(this_libmorda_dir)libmorda$(dot_so)

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

this_libmordavokne := mordavokne-$(this_render)
this_libmorda_render := morda-render-$(this_render)

this_libmordavokne := ../harness/modules/mordavokne/src/out/$(module_cfg)/lib$(this_libmordavokne)$(dot_so)
this_libmorda_render := ../harness/modules/$(this_libmorda_render)/src/out/$(module_cfg)/lib$(this_libmorda_render)$(dot_so)

ifeq ($(this_is_interactive),true)
    this_ldlibs += $(this_libmordavokne)
    this_ldlibs += $(this_libmorda_render)
endif

this_ldlibs += $(this_libmorda) -lpapki -ltreeml -lutki -lm

this_no_install := true

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_libmordavokne) $(this_libmorda)))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(this_libmorda_dir)

ifeq ($(this_is_interactive),true)
    this_run_name := $(notdir $(abspath $(d)))
    this_test_ld_path += $(dir $(this_libmordavokne)) $(dir $(this_libmorda_render))
    $(eval $(prorab-run))
else
    $(eval $(prorab-test))
endif
$(eval $(this_rules))

$(eval $(call prorab-include, ../harness/makefile))
