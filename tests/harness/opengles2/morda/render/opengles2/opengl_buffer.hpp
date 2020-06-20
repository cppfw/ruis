#pragma once

#include <utki/config.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif


namespace morda{

class opengl_buffer{
public:
	const GLuint buffer;

	opengl_buffer();
	
	opengl_buffer(const opengl_buffer&) = delete;
	opengl_buffer& operator=(const opengl_buffer&) = delete;
	
	virtual ~opengl_buffer()noexcept;
	
private:

};

}
