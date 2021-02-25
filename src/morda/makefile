include prorab.mk

this_name := morda

this_soname := $(shell cat $(d)../soname.txt)

this_srcs += $(call prorab-src-dir,morda)

$(eval $(call prorab-config, ../../config))

#    this_cxxflags += -DM_MORDA_RENDER_WIDGET_BORDERS

this_cxxflags += `pkg-config --cflags freetype2`

this_ldlibs += `pkg-config --libs freetype2`

this_ldlibs += -lpapki -ltreeml -lsvgdom -lsvgren -lutki -lpng -ljpeg -lm

ifeq ($(os), linux)
    this_ldlibs += -lpthread -lz
else ifeq ($(os), windows)
    this_ldlibs += -lz
else ifeq ($(os), macosx)
    this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib
endif

$(eval $(prorab-build-lib))
