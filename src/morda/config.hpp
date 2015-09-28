/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <utki/config.hpp>



#define M_MORDA_RENDER_OPENGL				0
#define M_MORDA_RENDER_OPENGLES				1
#define M_MORDA_RENDER_DIRECTX				2
#define M_MORDA_RENDER_GCM					3


#if M_OS == M_OS_LINUX

#	if M_OS_NAME == M_OS_NAME_ANDROID
#		define M_MORDA_RENDER M_MORDA_RENDER_OPENGLES
#	else
#		define M_MORDA_RENDER M_MORDA_RENDER_OPENGL
#	endif

#elif M_OS == M_OS_WINDOWS

#	ifdef M_MORDA_ENFORCE_OPENGL_UNDER_WINDOWS
#		define M_MORDA_RENDER M_MORDA_RENDER_OPENGL
#	else
#		define M_MORDA_RENDER M_MORDA_RENDER_DIRECTX
#	endif

#elif M_OS == M_OS_MACOSX
#	define M_MORDA_RENDER M_MORDA_RENDER_OPENGL

#else
#	error "unknown OS"
#endif



//#ifdef DEBUG
//#	define M_MORDA_RENDER_WIDGET_BORDERS
//#endif



namespace morda{

typedef float real;

}
