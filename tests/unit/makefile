include prorab.mk
include prorab-test.mk

include $(d)../harness/modules/module_cfg.mk

this_srcs += $(call prorab-src-dir, ../harness/util)

this_cxxflags += -isystem ../harness/modules/ruis-render-null/src

this__libruis_render_null_dir := ../harness/modules/ruis-render-null/src/out/$(module_cfg)/
this__libruis_render_null := $(this__libruis_render_null_dir)libruis-render-null$(dot_so)

this_ldlibs += -l tst
this_ldlibs += $(this__libruis_render_null)

include $(d)../common.mk
