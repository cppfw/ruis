this_name := tests

this_srcs += $(call prorab-src-dir, src)

$(eval $(call prorab-config, ../../config))

this_cxxflags += -I$(d)../../src/morda -I$(d)../harness/mordavokne

this_ldflags += -L$(d)../../src/morda/out/$(c)

ifeq ($(os),windows)
    this_ldlibs += -lmingw32 -mwindows # these should go first, otherwise linker will complain about undefined reference to WinMain
    this_ldlibs += -lglew32 -lopengl32 -lpng -ljpeg -lz -lfreetype
else ifeq ($(os),macosx)
    this_ldlibs += -framework OpenGL -framework Cocoa -lpng -ljpeg -lfreetype
    this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib

    this_ldflags += -rdynamic
else ifeq ($(os),linux)
    this_ldflags += -rdynamic
endif

ifeq ($(ren),gles2)
    this_render := opengles2
    this_mordavokne_lib := mordavokne-$(this_render)
else ifeq ($(filter-out gl2,$(ren)),)
    this_render := opengl2
    this_mordavokne_lib := mordavokne-$(this_render)
else
    $(error unknown value of 'ren': $(ren))
endif

ifeq ($(this_is_interactive),true)
    this_ldflags += -L$(d)../harness/$(this_render)/out/$(c)
    this_ldflags += -L$(d)../harness/mordavokne/out/$(c)
    this_ldlibs += -l$(this_mordavokne_lib)
    this_ldlibs += -lmorda-$(this_render)-ren
endif

this_ldlibs += -lmorda -lpapki -ltreeml -lutki -lm

this_no_install := true

$(eval $(prorab-build-app))

ifeq ($(this_is_interactive),true)
    define this_rules
        run_$(notdir $(patsubst %/,%,$(d))):: $(prorab_this_name)
$(.RECIPEPREFIX)@echo running $$^...
$(.RECIPEPREFIX)$(a)(cd $(d); LD_LIBRARY_PATH=../../src/morda/out/$(c):../harness/mordavokne/out/$(c):../harness/opengl2/out/$(c):../harness/opengles2/out/$(c) $$^)
    endef
else
    this_dirs := $(subst /, ,$(d))
    this_test := $(word $(words $(this_dirs)),$(this_dirs))

    define this_rules
        test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(this_test)
$(.RECIPEPREFIX)$(a)(cd $(d); LD_LIBRARY_PATH=../../src/morda/out/$(c) $$^)
$(.RECIPEPREFIX)@myci-passed.sh
    endef
endif
$(eval $(this_rules))

# add dependency on libmordavokne
$(prorab_this_name): $(abspath $(d)../harness/mordavokne/out/$(c)/lib$(this_mordavokne_lib)$(dot_so))

# add dependency on libmorda, libmordavokne
ifeq ($(os),windows)
$(prorab_this_name): $(d)$(this_out_dir)/libmorda$(dot_so) $(d)$(this_out_dir)/lib$(this_mordavokne_lib)$(dot_so) $(d)$(this_out_dir)/libmorda-opengl2-ren$(dot_so)

    define this_rules
        $(d)$(this_out_dir)/libmorda$(dot_so): $(abspath $(d)../../src/morda/out/$(c)/libmorda$(dot_so))
$(.RECIPEPREFIX)@echo "copy $$(notdir $$@)"
$(.RECIPEPREFIX)$(a)cp $$< $$@

        $(d)$(this_out_dir)/lib$(this_mordavokne_lib)$(dot_so): $(abspath $(d)../harness/mordavokne/out/$(c)/lib$(this_mordavokne_lib)$(dot_so))
$(.RECIPEPREFIX)@echo "copy $$(notdir $$@)"
$(.RECIPEPREFIX)$(a)cp $$< $$@

            $(d)$(this_out_dir)/libmorda-opengl2-ren$(dot_so): $(abspath $(d)../harness/opengl2/out/$(c)/libmorda-opengl2-ren$(dot_so))
$(.RECIPEPREFIX)@echo "copy $$(notdir $$@)"
$(.RECIPEPREFIX)$(a)cp $$< $$@

        clean::
$(.RECIPEPREFIX)$(a)rm -f $(d)$(this_out_dir)/libmorda$(dot_so)
    endef
    $(eval $(this_rules))
else
    $(prorab_this_name): $(abspath $(d)../../src/morda/out/$(c)/libmorda$(dot_so)) $(abspath $(d)../harness/mordavokne/out/$(c)/lib$(this_mordavokne_lib)$(dot_so))
endif

$(eval $(call prorab-include, ../../src/morda/makefile))
$(eval $(call prorab-include, ../harness/mordavokne/makefile))
