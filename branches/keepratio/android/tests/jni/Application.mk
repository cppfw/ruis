NDK_TOOLCHAIN_VERSION := 4.8

APP_PLATFORM := android-9

#we want to use exceptions
APP_CPPFLAGS += -fexceptions

#we want RTTI
APP_CPPFLAGS += -frtti

APP_CPPFLAGS += -std=c++11

#we want to use stl
APP_STL := gnustl_shared

APP_ABI := armeabi-v7a
