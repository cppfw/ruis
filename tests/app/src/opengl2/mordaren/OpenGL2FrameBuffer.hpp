#pragma once

#include <morda/render/frame_buffer.hpp>
#include <morda/render/Texture2D.hpp>

#include <GL/glew.h>

namespace mordaren{

class OpenGL2FrameBuffer : public morda::frame_buffer{
public:
	GLuint fbo;
	
	OpenGL2FrameBuffer(std::shared_ptr<morda::Texture2D> color);
	
	OpenGL2FrameBuffer(const OpenGL2FrameBuffer&) = delete;
	OpenGL2FrameBuffer& operator=(const OpenGL2FrameBuffer&) = delete;
	
	~OpenGL2FrameBuffer()noexcept;
private:

};

}
