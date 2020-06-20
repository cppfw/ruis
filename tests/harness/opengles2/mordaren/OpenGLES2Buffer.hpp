#pragma once

#include <utki/config.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif


namespace morda{

class OpenGLES2Buffer{
public:
	const GLuint buffer;

	OpenGLES2Buffer();
	
	OpenGLES2Buffer(const OpenGLES2Buffer&) = delete;
	OpenGLES2Buffer& operator=(const OpenGLES2Buffer&) = delete;
	
	virtual ~OpenGLES2Buffer()noexcept;
	
private:

};

}
