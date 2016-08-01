include prorab.mk


this_name := morda

this_soname := $(shell cat $(prorab_this_dir)/soname.txt)

this_srcs := $(shell find $(patsubst %/,%,$(prorab_this_dir)) -name *.cpp) #find all .cpp files
this_srcs := $(patsubst $(prorab_this_dir)%,%,$(this_srcs)) #remove this dir prefix

this_cxxflags += -Wall #enable all warnings
this_cxxflags += -Wno-comment #no warnings on nested comments
this_cxxflags += -funsigned-char #the 'char' type is unsigned
this_cxxflags += -Wnon-virtual-dtor #warn if base class has non-virtual destructor
this_cxxflags += -fno-operator-names #do not treat 'and', 'bitand','bitor','compl','not','or','xor' as keywords
this_cxxflags += -Werror #treat warnings as errors
this_cxxflags += -Wfatal-errors # stop on first error encountered
this_cxxflags += -fstrict-aliasing #strict aliasing (in order to comply with the c++ standard more strictly)
this_cxxflags += -g #include debugging symbols
this_cxxflags += -std=c++11

this_cxxflags += -D_GLIBCXX_USE_CXX11_ABI=0 #use old ABI


this_objcflags := 

ifeq ($(debug),true)
    this_cxxflags += -DDEBUG
#    this_cxxflags += -DM_MORDA_RENDER_WIDGET_BORDERS
    this_objcflags += -DDEBUG
else
    this_cxxflags += -O2 #TODO: switch to -O3 when g++ bug is fixed and it won't produce segfault
endif

ifeq ($(prorab_os), linux)
    this_cxxflags += -fPIC #generate position independent code
    this_cxxflags += `pkg-config --cflags freetype2`
    this_ldlibs += -lpthread -lX11 -lGL -lGLEW -ljpeg -lpng `pkg-config --libs freetype2` -lz
else ifeq ($(prorab_os), windows)
    this_cxxflags += -Wno-format #due to bugs in some g++ versions it warns about format when not needed, so disable format warnings
    this_cxxflags += -I/usr/include -I/usr/local/include/ -I/usr/local/include/freetype2
    this_cxxflags += -DM_MORDA_ENFORCE_OPENGL_UNDER_WINDOWS
    this_ldflags += -L/usr/local/lib -L/usr/lib

    #NOTE: order matters in MinGW, libs referenced by other libs should go after those libs which reference them.
    this_ldlibs += -lfreetype -ljpeg -lpng -lz -lopengl32 -lglew32 -lgdi32

    #WORKAROUND for MinGW bug:
    this_cxxflags += -D__STDC_FORMAT_MACROS
else ifeq ($(prorab_os), macosx)
    this_cxxflags += -stdlib=libc++ #this is needed to be able to use c++11 std lib
    this_cxxflags += -I/usr/local/include/freetype2
    this_ldlibs += -lc++
    this_ldlibs += -framework Cocoa -framework OpenGL -lpng -lfreetype -ljpeg -lGLEW
endif

this_ldlibs += -lnitki -lpogodi -lpapki -laika -lstob -lsvgdom -lsvgren

$(eval $(prorab-build-lib))
