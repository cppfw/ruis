#pragma once

#include <morda/render/FrameBuffer.hpp>
#include <morda/render/Texture2D.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

namespace mordaren{

class OpenGLES2FrameBuffer : public morda::FrameBuffer{
public:
	GLuint fbo;
	
	OpenGLES2FrameBuffer(std::shared_ptr<morda::Texture2D> color);
	
	OpenGLES2FrameBuffer(const OpenGLES2FrameBuffer&) = delete;
	OpenGLES2FrameBuffer& operator=(const OpenGLES2FrameBuffer&) = delete;
	
	~OpenGLES2FrameBuffer()noexcept;
private:

};

}
