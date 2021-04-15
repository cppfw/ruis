include prorab.mk

this_name := morda-probe

# use same so-name as morda library
this_soname := $(shell cat $(d)../soname.txt)

this_srcs += $(call prorab-src-dir, morda)

$(eval $(call prorab-config, ../../config))

# TODO: rewise libraries
ifeq ($(os), linux)
else ifeq ($(os), windows)
    this_ldlibs += -lz
else ifeq ($(os), macosx)
    this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib # TODO: not needed anymore?
endif

this_cxxflags += `pkg-config --cflags freetype2`

this_ldlibs += -lpthread `pkg-config --libs freetype2` -lz

this_ldlibs += -lpapki -ltreeml -lsvgdom -lsvgren -lpng -ljpeg -lm

# TODO: uncomment when probe is developed
# $(eval $(prorab-build-lib))
