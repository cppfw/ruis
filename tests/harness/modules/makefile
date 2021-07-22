include prorab.mk

modules_cfg := rel_no_install

# load config in order to resolve 'c' and 'config' in case of default config
$(eval $(call prorab-config, ../../../config))

define this_modules_deps
    $(call prorab-depend, \
            morda-render-$1/src/out/$(modules_cfg)/libmorda-render-$1$(dot_so), \
            ../../src/morda/out/$(c)/libmorda$(dot_so) \
        )
    $(call prorab-depend, \
            mordavokne/src/out/$(modules_cfg)/libmordavokne-$1$(dot_so), \
            morda-render-$1/src/out/$(modules_cfg)/libmorda-render-$1$(dot_so) \
            ../../src/morda/out/$(c)/libmorda$(dot_so) \
        )
endef
$(eval $(call this_modules_deps, opengl))
$(eval $(call this_modules_deps, opengles))

harness_old_cxxflags := $(CXXFLAGS)
harness_old_ldflags := $(LDLFAGS)
CXXFLAGS += -I$(d)../../src -I$(d)mordavokne/src
LDFLAGS += -L$(d)../../src/out/$(c) -L$(d)clargs/src/out/$(modules_cfg)
$(eval $(call prorab-include, mordavokne/src/makefile, $(modules_cfg)))
$(eval $(call prorab-include, morda-render-opengl/src/makefile, $(modules_cfg)))
$(eval $(call prorab-include, morda-render-opengles/src/makefile, $(modules_cfg)))
CXXFLAGS := $(harness_old_cxxflags)
LDFLAGS := $(harness_old_ldflags)
