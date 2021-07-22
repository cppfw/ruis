include prorab.mk

modules_cfg := rel_no_install

# load config in order to resolve 'c' and 'config' in case of default config
$(eval $(call prorab-config, ../../config))

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

