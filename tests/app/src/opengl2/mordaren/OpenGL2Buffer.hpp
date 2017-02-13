#pragma once

#include <GL/glew.h>

namespace mordaren{

class OpenGL2Buffer {
public:
	const GLuint buffer;

	OpenGL2Buffer();
	
	OpenGL2Buffer(const OpenGL2Buffer&) = delete;
	OpenGL2Buffer& operator=(const OpenGL2Buffer&) = delete;
	
	virtual ~OpenGL2Buffer()noexcept;
	
private:

};

}
