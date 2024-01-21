include prorab.mk

include $(d)module_cfg.mk

# load config in order to resolve 'c' and 'config' in case of default config
$(eval $(call prorab-config, ../../../config))

module_ruis_dir := ../../../src/
this_libruis_dir := $(module_ruis_dir)out/$(c)/
this_libruis := $(this_libruis_dir)libruis$(dot_so)

# save global flags
harness_old_cxxflags := $(CXXFLAGS)
harness_old_ldflags := $(LDLFAGS)

CXXFLAGS += -I$(abspath $(d)$(module_ruis_dir))
LDFLAGS += -L$(abspath $(d)$(this_libruis_dir))

define this_modules_deps
    CXXFLAGS += -I$(d)ruis-render-$(strip $1)/src
    LDFLAGS += -L$(d)ruis-render-$(strip $1)/src/out/$(module_cfg)
    $(call prorab-depend, \
            ruis-render-$(strip $1)/src/out/$(module_cfg)/libruis-render-$(strip $1)$(dot_so), \
            $(this_libruis) \
        )
    $(call prorab-depend, \
            ruisapp/src/out/$(module_cfg)/libruisapp-$(strip $1)$(dot_so), \
            ruis-render-$(strip $1)/src/out/$(module_cfg)/libruis-render-$(strip $1)$(dot_so) \
            $(this_libruis) \
        )
endef
$(eval $(call this_modules_deps, opengl))

ifeq ($(os),linux)
    $(eval $(call this_modules_deps, opengles))
endif

$(eval $(call prorab-try-include, ruisapp/src/makefile, $(module_cfg)))
$(eval $(call prorab-try-include, ruis-render-opengl/src/makefile, $(module_cfg)))

ifeq ($(os),linux)
    $(eval $(call prorab-try-include, ruis-render-opengles/src/makefile, $(module_cfg)))
endif

# restore global flags
CXXFLAGS := $(harness_old_cxxflags)
LDFLAGS := $(harness_old_ldflags)

$(eval $(call prorab-include, $(module_ruis_dir)makefile))

# clear global variable
module_ruis_dir :=
