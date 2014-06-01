LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#TARGET_BUILD_TYPE := debug

LOCAL_MODULE := morda

include $(LOCAL_PATH)/../../../../src/sources.mk

PREFIX := 

LOCAL_SRC_FILES := $(addprefix $(PREFIX),$(sources))

LOCAL_SRC_FILES += $(PREFIX)morda/glue/android/App.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/glue/android/AssetFile.cpp

LOCAL_CFLAGS := -Wno-div-by-zero #disable integer division by zero warning as it is sometimes useful when working with templates
LOCAL_CFLAGS += -DDEBUG

LOCAL_C_INCLUDES := 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../ting
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../stob
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libpng
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libjpeg/jpeg
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libfreetype2/include

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lz

#LOCAL_SHARED_LIBRARIES := ting stob

LOCAL_STATIC_LIBRARIES := png jpeg freetype2 ting stob

#disable undefined symbols check because we require CreateApp() function to be defined by library user
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
