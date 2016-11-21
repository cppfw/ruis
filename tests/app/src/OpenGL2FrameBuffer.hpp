#pragma once

#include "../../../src/morda/render/FrameBuffer_n.hpp"
#include "../../../src/morda/render/Texture2D_n.hpp"

#include <GL/glew.h>

class OpenGL2FrameBuffer : public morda::FrameBuffer_n{
public:
	GLuint fbo;
	
	OpenGL2FrameBuffer(std::shared_ptr<morda::Texture2D_n> color);
	
	OpenGL2FrameBuffer(const OpenGL2FrameBuffer&) = delete;
	OpenGL2FrameBuffer& operator=(const OpenGL2FrameBuffer&) = delete;
	
	~OpenGL2FrameBuffer()noexcept;
private:

};

