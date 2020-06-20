#pragma once

#include <morda/render/renderer.hpp>


#include "render_factory.hpp"


class renderer : public morda::Renderer{
	GLuint defaultFramebuffer;
public:
	renderer(std::unique_ptr<render_factory> factory = std::make_unique<render_factory>());
	
	renderer(const renderer& orig) = delete;
	renderer& operator=(const renderer& orig) = delete;
	
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

