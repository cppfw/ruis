include prorab.mk

include $(d)module_cfg.mk

# load config in order to resolve 'c' and 'config' in case of default config
$(eval $(call prorab-config, ../../../config))

module_morda_dir := ../../../src/
this_libmorda_dir := $(module_morda_dir)out/$(c)/
this_libmorda := $(this_libmorda_dir)libmorda$(dot_so)

# save global flags
harness_old_cxxflags := $(CXXFLAGS)
harness_old_ldflags := $(LDLFAGS)

CXXFLAGS += -I$(abspath $(d)$(module_morda_dir))
LDFLAGS += -L$(abspath $(d)$(this_libmorda_dir))

define this_modules_deps
    CXXFLAGS += -I$(d)morda-render-$(strip $1)/src
    LDFLAGS += -L$(d)morda-render-$(strip $1)/src/out/$(module_cfg)
    $(call prorab-depend, \
            morda-render-$(strip $1)/src/out/$(module_cfg)/libmorda-render-$(strip $1)$(dot_so), \
            $(this_libmorda) \
        )
    $(call prorab-depend, \
            mordavokne/src/out/$(module_cfg)/libmordavokne-$(strip $1)$(dot_so), \
            morda-render-$(strip $1)/src/out/$(module_cfg)/libmorda-render-$(strip $1)$(dot_so) \
            $(this_libmorda) \
        )
endef
$(eval $(call this_modules_deps, opengl))
$(eval $(call this_modules_deps, opengles))

$(eval $(call prorab-try-include, mordavokne/src/makefile, $(module_cfg)))
$(eval $(call prorab-try-include, morda-render-opengl/src/makefile, $(module_cfg)))

ifeq ($(os),linux)
    $(eval $(call prorab-try-include, morda-render-opengles/src/makefile, $(module_cfg)))
endif

# restore global flags
CXXFLAGS := $(harness_old_cxxflags)
LDFLAGS := $(harness_old_ldflags)

$(eval $(call prorab-include, $(module_morda_dir)makefile))

# clear global variable
module_morda_dir :=
