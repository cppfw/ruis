#pragma once

#include "../../../src/morda/render/FrameBuffer.hpp"
#include "../../../src/morda/render/Texture2D.hpp"

#include <GL/glew.h>

class OpenGL2FrameBuffer : public morda::FrameBuffer{
public:
	GLuint fbo;
	
	OpenGL2FrameBuffer(std::shared_ptr<morda::Texture2D> color);
	
	OpenGL2FrameBuffer(const OpenGL2FrameBuffer&) = delete;
	OpenGL2FrameBuffer& operator=(const OpenGL2FrameBuffer&) = delete;
	
	~OpenGL2FrameBuffer()noexcept;
private:

};

