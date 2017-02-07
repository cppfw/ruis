#pragma once

#include <GLES2/gl2.h>


namespace mordaren{

class OpenGLES2Buffer {
public:
	const GLuint buffer;

	OpenGLES2Buffer();
	
	OpenGLES2Buffer(const OpenGLES2Buffer&) = delete;
	OpenGLES2Buffer& operator=(const OpenGLES2Buffer&) = delete;
	
	virtual ~OpenGLES2Buffer()noexcept;
	
private:

};

}
