#include <utki/config.hpp>

#include "../App.hpp"


#include "friendAccessors.cppinc"


#if M_OS == M_OS_WINDOWS
#	include "windows/glue.cppinc"
#elif M_OS == M_OS_LINUX && M_OS_NAME == M_OS_NAME_ANDROID
#	include "android/glue.cppinc"
#elif M_OS == M_OS_LINUX
#	include "linux/glue.cppinc"
#endif
