#pragma once

#include <morda/render/FrameBuffer.hpp>
#include <morda/render/Texture2D.hpp>

#ifdef M_RENDER_OPENGLES2
#	include <GLES2/gl2.h>
#else
#	include <GL/glew.h>
#endif

class OpenGL2FrameBuffer : public morda::FrameBuffer{
public:
	GLuint fbo;
	
	OpenGL2FrameBuffer(std::shared_ptr<morda::Texture2D> color);
	
	OpenGL2FrameBuffer(const OpenGL2FrameBuffer&) = delete;
	OpenGL2FrameBuffer& operator=(const OpenGL2FrameBuffer&) = delete;
	
	~OpenGL2FrameBuffer()noexcept;
private:

};

