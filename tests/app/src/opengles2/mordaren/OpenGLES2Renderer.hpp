#pragma once

#include <morda/render/Renderer.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

#include "OpenGLES2Factory.hpp"

namespace mordaren{

class OpenGLES2Renderer : public morda::Renderer{
	GLuint defaultFramebuffer;
public:
	OpenGLES2Renderer(std::unique_ptr<OpenGLES2Factory> factory = utki::makeUnique<OpenGLES2Factory>());
	
	OpenGLES2Renderer(const OpenGLES2Renderer& orig) = delete;
	OpenGLES2Renderer& operator=(const OpenGLES2Renderer& orig) = delete;
	
	void setFramebufferInternal(morda::FrameBuffer* fb) override;

	void clearFramebuffer()override;
	
	bool isScissorEnabled() const override;
	
	void setScissorEnabled(bool enabled) override;
	
	kolme::Recti getScissorRect() const override;
	
	void setScissorRect(kolme::Recti r) override;

	kolme::Recti getViewport()const override;
	
	void setViewport(kolme::Recti r) override;
	
	void setBlendEnabled(bool enable) override;

	void setBlendFunc(BlendFactor_e srcClr, BlendFactor_e dstClr, BlendFactor_e srcAlpha, BlendFactor_e dstAlpha) override;

};

}
