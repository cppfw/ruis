include prorab.mk

ifeq ($(os),linux)

this_name := morda-opengles2-ren

this_soname := 0

$(eval $(call prorab-config, ../../../config))

this_srcs += $(call prorab-src-dir, .)

this_cxxflags += -I$(d)../../../src/morda

this_ldlibs += -lmorda -lstdc++
this_ldlibs += `pkg-config --libs glesv2`
this_ldflags += -L$(d)../../../src/morda/out/$(c)

ifeq ($(os), linux)
    this_cxxflags += -fPIC
    this_ldlibs +=
else ifeq ($(os), macosx)
    this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib
    this_ldlibs += -lc++
else ifeq ($(os),windows)
endif

this_no_install := true

$(eval $(prorab-build-lib))

# add dependency on libmorda
$(prorab_this_name): $(abspath $(d)../../../src/morda/out/$(c)/libmorda$(dot_so))

$(eval $(call prorab-include, ../../../src/morda/makefile))

endif # build only under linux
