LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := freetype2

LOCAL_CFLAGS := -DANDROID_NDK \
		-DFT2_BUILD_LIBRARY=1

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src

LOCAL_SRC_FILES := src/autofit/autofit.c
LOCAL_SRC_FILES += src/base/basepic.c
LOCAL_SRC_FILES += src/base/ftapi.c
LOCAL_SRC_FILES += src/base/ftbase.c
LOCAL_SRC_FILES += src/base/ftbbox.c
LOCAL_SRC_FILES += src/base/ftbitmap.c
LOCAL_SRC_FILES += src/base/ftdbgmem.c
LOCAL_SRC_FILES += src/base/ftdebug.c
LOCAL_SRC_FILES += src/base/ftglyph.c
LOCAL_SRC_FILES += src/base/ftinit.c
LOCAL_SRC_FILES += src/base/ftpic.c
LOCAL_SRC_FILES += src/base/ftstroke.c
LOCAL_SRC_FILES += src/base/ftsynth.c
LOCAL_SRC_FILES += src/base/ftsystem.c
LOCAL_SRC_FILES += src/cff/cff.c
LOCAL_SRC_FILES += src/pshinter/pshinter.c
LOCAL_SRC_FILES += src/psnames/psnames.c
LOCAL_SRC_FILES += src/raster/raster.c
LOCAL_SRC_FILES += src/sfnt/sfnt.c
LOCAL_SRC_FILES += src/smooth/smooth.c
LOCAL_SRC_FILES += src/truetype/truetype.c

LOCAL_LDLIBS := -ldl -llog

include $(BUILD_STATIC_LIBRARY)

