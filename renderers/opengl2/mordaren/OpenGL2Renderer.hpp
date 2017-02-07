#pragma once

#include <GL/glew.h>

#include <morda/render/Renderer.hpp>

#include "OpenGL2Factory.hpp"

namespace mordaren{

class OpenGL2Renderer : public morda::Renderer{
	GLuint defaultFramebuffer;
public:
	OpenGL2Renderer(std::unique_ptr<OpenGL2Factory> factory = utki::makeUnique<OpenGL2Factory>());
	
	OpenGL2Renderer(const OpenGL2Renderer& orig) = delete;
	OpenGL2Renderer& operator=(const OpenGL2Renderer& orig) = delete;
	
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
