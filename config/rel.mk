include $(config_dir)base/base.mk

this_cxxflags += -O3

# TODO: compilation with -O3 currently fails with unjustified "maybe used uninitialized" error
#       when building with "g++ (Debian 12.2.0-14) 12.2.0",
#       have't tried it with clang++. Need to remove this warning suppression when GCC is fixed.
ifeq ($(this_cxx),g++)
    ifeq ($(os),macosx)
        # On macos, the this_cxx can be set to g++, but it is still clang compiler, becuase
        # often, on macos the g++ is symlinked to clang++.
        # Thus, on macos do not suppress the warning, because it is not supported by clang. 
    else
        this_cxxflags += -Wno-maybe-uninitialized
    endif
endif

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
