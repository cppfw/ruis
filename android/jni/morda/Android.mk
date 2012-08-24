LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#TARGET_BUILD_TYPE := debug

LOCAL_MODULE := morda

LOCAL_SRC_FILES := morda/main.cpp
#LOCAL_SRC_FILES += ../AssetFile/AssetFile.cpp
#LOCAL_SRC_FILES += Activity.cpp

LOCAL_CFLAGS := -Wno-div-by-zero #disable integer division by zero warning as it is sometimes useful when working with templates
LOCAL_CFLAGS += -DDEBUG

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../ting $(LOCAL_PATH)/../stob $(LOCAL_PATH)/../tride

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM

LOCAL_SHARED_LIBRARIES := ting stob

include $(BUILD_SHARED_LIBRARY)
