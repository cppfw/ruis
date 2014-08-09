include prorab.mk


this_name := morda

this_so_name := 0


#Sources
include $(prorab_this_dir)sources.mk

ifeq ($(prorab_os),linux)
    this_srcs += morda/glue/linux/App.cpp
else ifeq ($(prorab_os), macosx)
    this_srcs += morda/glue/macosx/main.mm
else ifeq ($(prorab_os), windows)
    this_srcs += morda/glue/windows/App.cpp
    this_srcs += morda/glue/windows/main.cpp
endif



this_cflags += -Wall #enable all warnings
this_cflags += -Wno-comment #no warnings on nested comments
this_cflags += -funsigned-char #the 'char' type is unsigned
this_cflags += -Wnon-virtual-dtor #warn if base class has non-virtual destructor
this_cflags += -fno-operator-names #do not treat 'and', 'bitand','bitor','compl','not','or','xor' as keywords
this_cflags += -Werror #treat warnings as errors
this_cflags += -Wfatal-errors # stop on first error encountered
this_cflags += -fstrict-aliasing #strict aliasing (in order to comply with the c++ standard more strictly)
this_cflags += -g #include debugging symbols
                #-P -E #-Wreorder
                #-O3 -funroll-loops -fomit-frame-pointer
this_cflags += -std=c++11


ifeq ($(debug),true)
    this_cflags += -DDEBUG
endif

ifneq ($(prorab_os),windows)
    this_cflags += -fPIC # Since we are building shared library, we need Position-Independend Code
endif

ifeq ($(prorab_os), linux)
    this_cflags += `pkg-config --cflags freetype2`
    this_ldlibs += -lting -lpthread -lX11 -lGL -lGLEW -ljpeg -lpng `pkg-config --libs freetype2` -lz -lstob
else ifeq ($(prorab_os), windows)
    this_cflags += -Wno-format #due to bugs in some g++ versions it warns about format when not needed, so disable format warnings
    this_cflags += -I/usr/include -I/usr/local/include/ -I/usr/local/include/freetype2
    this_ldflags += -L/usr/local/lib -L/usr/lib
    this_ldlibs += -lfreetype -lting -ljpeg -lpng -lz -lopengl32 -lglew32 -lgdi32 -lstob
else ifeq ($(prorab_os), macosx)
    this_ldlibs += -framework Cocoa -framework OpenGL -lting -lpng -lfreetype -ljpeg -lstob -lGLEW
endif



$(eval $(prorab-build-lib))

