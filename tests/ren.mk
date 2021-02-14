this_cxxflags += -I$(d)../../src/morda -I$(d)../harness/mordavokne

this_ldflags += -L$(d)../../src/morda/out/$(c)
this_ldflags += -L$(d)../harness/mordavokne/out/$(c)

ifeq ($(ren),gles2)
    this_mordavokne_lib := mordavokne-opengles2
else
    this_mordavokne_lib := mordavokne-opengl2
endif

this_ldlibs += -l$(this_mordavokne_lib)
