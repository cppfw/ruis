include prorab.mk

include $(d)module_cfg.mk

# load config in order to resolve 'c' and 'config' in case of default config
$(eval $(call prorab-config, ../../../config))

module_ruis_dir := ../../../src/
this__libruis_dir := $(module_ruis_dir)out/$(c)/
this__libruis := $(this__libruis_dir)libruis$(dot_so)

# save global flags
harness_old_cxxflags := $(CXXFLAGS)
harness_old_ldflags := $(LDFLAGS)

CXXFLAGS += -I$(abspath $(d)$(module_ruis_dir))
LDFLAGS += -L$(abspath $(d)$(this__libruis_dir))

CXXFLAGS += -isystem $(d)ruis-render-null/src
LDFLAGS += -L $(d)ruis-render-null/src/out/$(module_cfg)
$(call prorab-depend, \
        ruis-render-null/src/out/$(module_cfg)/libruis-render-null$(dot_so), \
        $(this__libruis) \
    )

define this__modules_deps
    CXXFLAGS += -isystem $(d)ruis-render-$(strip $1)/src
    LDFLAGS += -L $(d)ruis-render-$(strip $1)/src/out/$(module_cfg)
    $(call prorab-depend, \
            ruis-render-$(strip $1)/src/out/$(module_cfg)/libruis-render-$(strip $1)$(dot_so), \
            $(this__libruis) \
        )
    $(call prorab-depend, \
            ruisapp/src/out/$(module_cfg)/$(strip $1)$(if $2,-$2)/libruisapp-$(strip $1)$(if $2,-$2)$(dot_so), \
            ruis-render-$(strip $1)/src/out/$(module_cfg)/libruis-render-$(strip $1)$(dot_so) \
            $(this__libruis) \
        )
endef

ifeq ($(os),linux)
    $(eval $(call this__modules_deps,opengl,xorg))
    $(eval $(call this__modules_deps,opengles,xorg))
    $(eval $(call this__modules_deps,opengl,wayland))
    $(eval $(call this__modules_deps,opengles,wayland))
else
    $(eval $(call this__modules_deps,opengl,))
endif

# use prorab-try-include because on macos homebrew will try to build from source when installing,
# and of course, it will not have git submodules in the source archive
$(eval $(call prorab-try-include, ruisapp/src/makefile, $(module_cfg)))
$(eval $(call prorab-try-include, ruis-render-opengl/src/makefile, $(module_cfg)))
$(eval $(call prorab-try-include, ruis-render-null/src/makefile, $(module_cfg)))
ifeq ($(os),linux)
    $(eval $(call prorab-try-include, ruis-render-opengles/src/makefile, $(module_cfg)))
endif

# restore global flags
CXXFLAGS := $(harness_old_cxxflags)
LDFLAGS := $(harness_old_ldflags)

$(eval $(call prorab-include, $(module_ruis_dir)makefile))

# clear global variable
module_ruis_dir :=
