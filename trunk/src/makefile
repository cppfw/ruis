# Author: Ivan Gagis <igagis@gmail.com>

#==============================================================================
#==============================================================================
#==============================================================================
#                        Project configuration part
#             change these strings to configure project building

#default platform. It may be overriden by specifying platform=xxx in command line when running make
#Known platforms are:
#    linux
#    maemo_linux
#    win32
#    win32cross
platform := linux

#build configuration:
#    debug
#    release
build_config := release

name := libmorda

so_name := 0

ifeq ($(platform), linux)
    name := $(name).so
endif
ifeq ($(platform), win32)
    name := $(name).dll
endif
ifeq ($(platform), win32cross)
    name := $(name).dll
endif



#Sources
srcs :=
srcs += morda/Application.cpp




defines :=

ifeq ($(build_config), debug)
    defines += -DDEBUG
endif

ifeq ($(platform), maemo_linux)
    defines += -DM_MAEMO
endif



compiler_flags := -Wall #enable all warnings
compiler_flags += -Wno-comment #no warnings on nested comments
compiler_flags += -funsigned-char #the 'char' type is unsigned
compiler_flags += -Wnon-virtual-dtor #warn if base class has non-virtual destructor
compiler_flags += -fno-operator-names #do not treat 'and', 'bitand','bitor','compl','not','or','xor' as keywords
compiler_flags += -Werror #treat warnings as errors
compiler_flags += -Wfatal-errors # stop on first error encountered
compiler_flags += -fstrict-aliasing #strict aliasing (in order to comply with the c++ standard more strictly)
compiler_flags += -g #include debugging symbols
                #-P -E #-Wreorder
                #-O3 -funroll-loops -fomit-frame-pointer
#compiler_flags += -std=c++0x



ifeq ($(platform), linux)
    compiler_flags += -fPIC # Since we are building shared library, we need Position-Independend Code
endif
ifeq ($(platform), maemo_linux)
    compiler_flags += -fPIC # Since we are building shared library, we need Position-Independend Code
endif


linker_flags := -shared

ifeq ($(platform), linux)
    linker_flags += -Wl,-soname,$(name).$(so_name)
endif
ifeq ($(platform), win32)
    linker_flags += -s
endif
ifeq ($(platform), win32cross)
    linker_flags +=
endif



#=======================
#  Include directories
#=======================
ifeq ($(platform), maemo_linux)
    include_dirs :=
endif
ifeq ($(platform), linux)
    include_dirs :=
endif
ifeq ($(platform), win32)
    include_dirs :=
endif
ifeq ($(platform), win32cross)
    include_dirs :=
endif



#============================
#  Libs and lib directories
#============================
ifeq ($(platform), maemo_linux)
    libs := 
endif
ifeq ($(platform), linux)
    libs := -lting -lpthread -lX11 -lGL
endif
ifeq ($(platform), win32)
    libs := 
endif
ifeq ($(platform), win32cross)
    libs :=
endif



remove_on_clean:=



#                     __
#                    /  \__/  end of configuration part
#==============================================================================

#remove program
ifeq ($(platform),win32)
    remove := del /Q /F
else
    remove := rm -f
endif

obj_dir := obj/1/2
compiler := g++


#==============================================================================
#=============================TARGETS==========================================
#==============================================================================

#==========================
#=project default target=
default: create-obj-dir $(name)

create-obj-dir:
ifeq ($(platform), win32)
	@if not exist $(obj_dir) mkdir $(subst /,\,$(obj_dir))
else
	@mkdir -p $(obj_dir)
endif



#find all .cpp files and get an .o file name for it to get dependencies for this target
$(name): $(addprefix $(obj_dir)/,$(patsubst %.cpp,%.o,$(srcs)))
	@echo Linking $@...
ifeq ($(platform), win32)
	@ar cr $(patsubst %.dll,%.a,$@) $^
	@$(compiler) $^ -o "$@" $(libs) $(linker_flags)
else
	@ar cr $(patsubst %.so,%.a,$@) $^
	@$(compiler) $^ -o "$@".$(so_name) $(libs) $(linker_flags)
	@ln -f -s $@.$(so_name) $@
endif


#======================
#=compile .cpp sources=
$(obj_dir)/%.o:%.cpp
	@echo Compiling $<...
ifeq ($(platform), win32)
	@if not exist $(dir $@) mkdir $(subst /,\,$(dir $@))
else
	@mkdir -p $(dir $@)
endif
# -MF option specifies dependency output file name
	@$(compiler) -c -MF "$(patsubst %.o,%.d,$@)" -MD -o "$@" $(compiler_flags) $(defines) $(include_dirs) $<
#workaround for gcc 4.2 compiler (it behaves differently than gcc 4.1 for some reason)
#ifeq ($(platform), linux)
#	@echo -n $(obj_dir)/ > $(patsubst %.o,%.d_,$@)
#	@cat $(patsubst %.o,%.d,$@) >> $(patsubst %.o,%.d_,$@)
#	@mv $(patsubst %.o,%.d_,$@) $(patsubst %.o,%.d,$@)
#endif

include $(wildcard $(addsuffix /*.d,$(dir $(addprefix $(obj_dir)/,$(srcs)))))



#==================
#=build all target=
all: clean proj


test:

#==============
#=clean target=
#delete all objects and executables

#it is an abstract target (not a file), declare as .PHONY
.PHONY: clean
clean:
ifeq ($(platform),win32)
	@$(remove) $(name)
	@rmdir /S /Q obj
else
	@$(remove) $(name)
	@$(remove) $(patsubst %.so,%.a,$(name))
	@$(remove) $(name).$(so_name)
	@$(remove) -rf $(dir $(obj_dir))
	@$(remove) -rf $(remove_on_clean)
endif

include ../targets.mk
