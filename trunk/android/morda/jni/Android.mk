LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#TARGET_BUILD_TYPE := debug

LOCAL_MODULE := morda

PREFIX := ../../../src/

LOCAL_SRC_FILES := $(PREFIX)morda/android/App.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/android/AssetFile.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/App.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/Container.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/GuiInflater.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/Layout.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/Updateable.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/Widget.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/shaders/Shader.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/shaders/SimpleSingleColoringShader.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/shaders/TexturingShader.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/shaders/SimpleTexturingShader.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/resman/ResourceManager.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/resman/Resource.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/resources/ResTexture.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/resources/ResFont.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/util/GLTexture.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/util/Gravity.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/util/LeftBottomRightTop.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/util/Image.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/util/TexFont.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/util/util.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/widgets/AbstractButton.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/widgets/Button.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/widgets/Label.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/layouts/LinearLayout.cpp
LOCAL_SRC_FILES += $(PREFIX)morda/layouts/FrameLayout.cpp


LOCAL_CFLAGS := -Wno-div-by-zero #disable integer division by zero warning as it is sometimes useful when working with templates
LOCAL_CFLAGS += -DDEBUG

LOCAL_C_INCLUDES := 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../ting
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../stob
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libpng
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libjpeg/jpeg
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libfreetype2/include

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lz

#LOCAL_SHARED_LIBRARIES := ting stob

LOCAL_STATIC_LIBRARIES := png jpeg freetype2 ting stob

#disable undefined symbols check because we require CreateApp() function to be defined by library user
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
