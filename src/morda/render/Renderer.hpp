#pragma once

#include "RenderFactory.hpp"

namespace morda{

class Renderer : virtual public utki::Shared{
public:
	const std::unique_ptr<RenderFactory> factory;
	
	const std::unique_ptr<RenderFactory::Shaders> shader;
	
public:
	const std::shared_ptr<VertexBuffer> quad01VBO;
	const std::shared_ptr<IndexBuffer> quadIndices;

	const std::shared_ptr<VertexArray> posQuad01VAO;
	
	const std::shared_ptr<VertexArray> posTexQuad01VAO;
	
protected:
	struct Params{
		unsigned maxTextureSize = 2048;
		bool isRighthanded = true;
	};
	
	Renderer(std::unique_ptr<RenderFactory> factory, const Params& params);
	
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	
private:
	std::shared_ptr<FrameBuffer> curFB;
public:
	const unsigned maxTextureSize;
	const bool isRighthanded;
	
	//can be nullptr = set screen framebuffer
	void setFramebuffer(std::shared_ptr<FrameBuffer> fb);
	
	virtual void clearFramebuffer() = 0;
	
	virtual bool isScissorEnabled()const = 0;
	
	virtual void setScissorEnabled(bool enabled) = 0;
	
	virtual kolme::Recti getScissorRect()const = 0;
	
	virtual void setScissorRect(kolme::Recti r) = 0;
	
	virtual kolme::Recti getViewport()const = 0;
	
	virtual void setViewport(kolme::Recti r) = 0;
	
	virtual void setBlendEnabled(bool enable) = 0;
	
	/**
	 * @brief Blending factor type.
	 * Enumeration defines possible blending factor types.
	 */
	enum class BlendFactor_e{
		//NOTE: do not change order
		
		ZERO,
		ONE,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,
		CONSTANT_COLOR,
		ONE_MINUS_CONSTANT_COLOR,
		CONSTANT_ALPHA,
		ONE_MINUS_CONSTANT_ALPHA,
		SRC_ALPHA_SATURATE
	};
	
	virtual void setBlendFunc(BlendFactor_e srcClr, BlendFactor_e dstClr, BlendFactor_e srcAlpha, BlendFactor_e dstAlpha) = 0;
	
protected:
	virtual void setFramebufferInternal(FrameBuffer* fb) = 0;
};

}
