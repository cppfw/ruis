LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#TARGET_BUILD_TYPE := debug

LOCAL_MODULE := tests

PREFIX := ../../../tests/

LOCAL_SRC_FILES := $(PREFIX)app/src/main.cpp


LOCAL_CFLAGS := -Wno-div-by-zero #disable integer division by zero warning as it is sometimes useful when working with templates
LOCAL_CFLAGS += -DDEBUG

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../ting/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../stob

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lz

#LOCAL_SHARED_LIBRARIES := stob ting

LOCAL_STATIC_LIBRARIES := morda ting stob

#disable undefined symbols check because we require CreateApp() function to be defined by library user
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

include $(BUILD_SHARED_LIBRARY)
