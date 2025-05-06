include $(config_dir)base/base.mk

this_cxxflags += -DDEBUG
this_cxxflags += -O0

this_dbg := -dbg

# do not install headers
this_install_hdrs := $(prorab_space)
