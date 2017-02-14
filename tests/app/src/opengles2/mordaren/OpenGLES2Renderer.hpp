#pragma once

#include <utki/config.hpp>

#include <morda/render/Renderer.hpp>

#include "OpenGLES2Factory.hpp"

namespace mordaren{

class OpenGLES2Renderer : public morda::Renderer{
	bool defaultFramebufferInitialized = false;
	std::uint32_t defaultFramebuffer; //NOTE: GLuint is fixed 32bit type, according to OpenGL specs, so use std::uint32_t.
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
