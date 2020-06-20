#pragma once

#include <utki/config.hpp>

#include <morda/render/frame_buffer.hpp>
#include <morda/render/texture_2d.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

namespace morda{

class OpenGLES2FrameBuffer : public morda::frame_buffer{
public:
	GLuint fbo;
	
	OpenGLES2FrameBuffer(std::shared_ptr<morda::texture_2d> color);
	
	OpenGLES2FrameBuffer(const OpenGLES2FrameBuffer&) = delete;
	OpenGLES2FrameBuffer& operator=(const OpenGLES2FrameBuffer&) = delete;
	
	~OpenGLES2FrameBuffer()noexcept;
private:

};

}
