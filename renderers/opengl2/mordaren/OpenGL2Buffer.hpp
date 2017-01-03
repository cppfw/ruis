#pragma once

#ifdef M_RENDER_OPENGLES2
#	include <GLES2/gl2.h>
#else
#	include <GL/glew.h>
#endif

class OpenGL2Buffer {
public:
	const GLuint buffer;

	OpenGL2Buffer();
	
	OpenGL2Buffer(const OpenGL2Buffer&) = delete;
	OpenGL2Buffer& operator=(const OpenGL2Buffer&) = delete;
	
	virtual ~OpenGL2Buffer()noexcept;
	
private:

};


