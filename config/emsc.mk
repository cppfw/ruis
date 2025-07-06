include $(config_dir)base/base.mk

this_cxxflags += -O3

this_cxx := em++
this_cc := emcc
this_ar := emar

this_static_lib_only := true

this_cxxflags += -fwasm-exceptions
this_ldflags += -fwasm-exceptions

this_cxxflags += -pthread
this_ldflags += -pthread