#pragma once

#include <GL/glew.h>

namespace morda{ namespace render_opengl2{

class opengl_buffer {
public:
	const GLuint buffer;

	opengl_buffer();
	
	opengl_buffer(const opengl_buffer&) = delete;
	opengl_buffer& operator=(const opengl_buffer&) = delete;
	
	virtual ~opengl_buffer()noexcept;
	
private:

};

}}
