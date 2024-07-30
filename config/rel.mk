include $(config_dir)base/base.mk

# TODO: compilation with -O3 currently fails with unjjustified "maybe used uninitialized" error
#       when building with "g++ (Debian 12.2.0-14) 12.2.0",
#       have't tried it with clang++. Need to restore -O3 when GCC is fixed.
this_cxxflags += -O2

this_lint_cmd = $(prorab_lint_cmd_clang_tidy)

# WORKAROUND: on ubuntu jammy dpkg-buildpackage passes -ffat-lto-objects compilation flag
# which is not supported by clang and clang-tidy complains about it:
# error: optimization flag '-ffat-lto-objects' is not supported [clang-diagnostic-ignored-optimization-argument]
# Thus, suppress this warning.
this_cxxflags += -Wno-ignored-optimization-argument

ifeq ($(os),macosx)
    # WORKAROUND:
    # clang-tidy on macos doesn't use /usr/local/include as default place to
    # search for header files, so we add it explicitly
    this_cxxflags += -isystem /usr/local/include
endif
