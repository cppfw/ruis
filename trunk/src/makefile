include prorab.mk


this_name := morda

this_soname := $(shell cat $(prorab_this_dir)/soname.txt)


#Sources
include $(prorab_this_dir)sources.mk

ifeq ($(prorab_os),linux)
    this_srcs += morda/glue/linux/main.cpp
else ifeq ($(prorab_os), macosx)
    this_mm_src := morda/glue/macosx/main.mm
else ifeq ($(prorab_os), windows)
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


this_objcflags := 

ifeq ($(debug),true)
    this_cflags += -DDEBUG
#    this_cflags += -DM_MORDA_RENDER_WIDGET_BORDERS
    this_objcflags += -DDEBUG
endif

ifeq ($(prorab_os), linux)
    this_cflags += -fPIC
    this_cflags += `pkg-config --cflags freetype2`
    this_ldlibs += -lting -lpthread -lX11 -lGL -lGLEW -ljpeg -lpng `pkg-config --libs freetype2` -lz -lstob
else ifeq ($(prorab_os), windows)
    this_cflags += -Wno-format #due to bugs in some g++ versions it warns about format when not needed, so disable format warnings
    this_cflags += -I/usr/include -I/usr/local/include/ -I/usr/local/include/freetype2
    this_ldflags += -L/usr/local/lib -L/usr/lib

    #NOTE: order matters in MinGW, libs referenced by other libs should go after those libs which reference them.
    this_ldlibs += -lfreetype -lstob -lting -ljpeg -lpng -lz -lopengl32 -lglew32 -lgdi32

    #WORKAROUND for MinGW bug:
    this_cflags += -D__STDC_FORMAT_MACROS
else ifeq ($(prorab_os), macosx)
    this_cflags += -stdlib=libc++ #this is needed to be able to use c++11 std lib
    this_ldlibs += -lc++
    this_ldlibs += -framework Cocoa -framework OpenGL -lting -lpng -lfreetype -ljpeg -lstob -lGLEW
    
    this_mm_obj := $(prorab_this_dir)$(prorab_obj_dir)morda/glue/macosx/main.o

    define this_rules
        $(this_mm_obj): $(prorab_this_dir)morda/glue/macosx/main.mm
		@echo Compiling $$<...
		$(prorab_echo)mkdir -p $$(dir $$@)
		$(prorab_echo)$(CC) -ObjC++ -std=c++11 -c -o "$$@" $(this_objcflags) $$<
    endef
    $(eval $(this_rules))
endif


$(eval $(prorab-build-lib))


ifeq ($(prorab_os), macosx)
    $(prorab_this_staticlib): $(this_mm_obj)

    $(prorab_this_name): $(this_mm_obj)
endif
