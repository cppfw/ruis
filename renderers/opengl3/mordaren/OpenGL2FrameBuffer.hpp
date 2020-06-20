#pragma once

#include <morda/render/FrameBuffer.hpp>
#include <morda/render/Texture2D.hpp>

#include <GL/glew.h>

class frame_buffer : public morda::FrameBuffer{
public:
	GLuint fbo;
	
	frame_buffer(std::shared_ptr<morda::Texture2D> color);
	
	frame_buffer(const frame_buffer&) = delete;
	frame_buffer& operator=(const frame_buffer&) = delete;
	
	~frame_buffer()noexcept;
private:

};

