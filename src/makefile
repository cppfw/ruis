include prorab.mk
include prorab-license.mk
include prorab-install-dbgsrc.mk
include prorab-clang-format.mk

this_name := morda

this_soname := $(shell cat $(d)soname.txt)

this_srcs += $(call prorab-src-dir, morda)

$(eval $(call prorab-config, ../config))

#    this_cxxflags += -DM_MORDA_RENDER_WIDGET_BORDERS

ifeq ($(os),windows)
    this_cxxflags += $(shell pkg-config --cflags freetype2)
else
    # use -isystem instead of -I for freetype because otherwise clang-tidy goes inside
    # of freetype headers and finds a lot of findings there
    this_cxxflags += $(subst -I,-isystem,$(shell pkg-config --cflags freetype2))
endif

this_ldlibs += $(shell pkg-config --libs freetype2)

this_ldlibs += -lpapki -ltreeml -lsvgdom -lsvgren -lutki -lpng -ljpeg -lm

$(eval $(prorab-build-lib))

this_src_dir := $(this_name)
this_license_file := ../LICENSE
$(eval $(prorab-license))

$(eval $(prorab-clang-format))

$(eval $(prorab-install-dbgsrc))
