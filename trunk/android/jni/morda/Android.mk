LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#TARGET_BUILD_TYPE := debug

LOCAL_MODULE := morda

LOCAL_SRC_FILES := morda/android/App.cpp
LOCAL_SRC_FILES += morda/android/AssetFile.cpp
LOCAL_SRC_FILES += morda/App.cpp
LOCAL_SRC_FILES += morda/Widget.cpp
LOCAL_SRC_FILES += morda/Container.cpp
LOCAL_SRC_FILES += morda/GuiInflater.cpp
LOCAL_SRC_FILES += morda/shaders/Shader.cpp
LOCAL_SRC_FILES += morda/shaders/SimpleSingleColoringShader.cpp
LOCAL_SRC_FILES += morda/shaders/TexturingShader.cpp
LOCAL_SRC_FILES += morda/shaders/SimpleTexturingShader.cpp
LOCAL_SRC_FILES += morda/resman/ResourceManager.cpp
LOCAL_SRC_FILES += morda/resman/Resource.cpp
LOCAL_SRC_FILES += morda/resources/ResTexture.cpp
LOCAL_SRC_FILES += morda/resources/ResFont.cpp
LOCAL_SRC_FILES += morda/util/GLTexture.cpp
LOCAL_SRC_FILES += morda/util/Image.cpp
LOCAL_SRC_FILES += morda/util/TexFont.cpp
LOCAL_SRC_FILES += morda/util/util.cpp
LOCAL_SRC_FILES += morda/widgets/AbstractButton.cpp
LOCAL_SRC_FILES += morda/widgets/Button.cpp
LOCAL_SRC_FILES += morda/widgets/Label.cpp
LOCAL_SRC_FILES += morda/layouts/LinearLayout.cpp


LOCAL_CFLAGS := -Wno-div-by-zero #disable integer division by zero warning as it is sometimes useful when working with templates
LOCAL_CFLAGS += -DDEBUG

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../ting $(LOCAL_PATH)/../stob $(LOCAL_PATH)/../tride

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM

LOCAL_SHARED_LIBRARIES := ting stob

#disable undefined symbols check because we require CreateApp() function to be defined by library user
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

include $(BUILD_SHARED_LIBRARY)
