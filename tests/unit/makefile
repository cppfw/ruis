include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := unit_tests

this_srcs := $(call prorab-src-dir, src)

this_cxxflags += -I../../src/morda

this_ldlibs += -ltst -lutki -lmorda

$(eval $(prorab-build-app))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(prorab_space)
$(eval $(prorab-test))
