#pragma once

#include "RenderFactory.hpp"

namespace morda{

class renderer : virtual public utki::shared{
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
		r4::mat4f initialMatrix = r4::mat4f().identity();
	};
	
	renderer(std::unique_ptr<RenderFactory> factory, const Params& params);
	
	renderer(const renderer&) = delete;
	renderer& operator=(const renderer&) = delete;
	
private:
	std::shared_ptr<FrameBuffer> curFB;
public:
	const unsigned maxTextureSize;
	
	/**
	 * @brief Initial matrix to use for rendering.
	 * This is the matrix which makes screen edges to be: left = -1, right = 1, top = 1, bottom = -1.
	 */
	const r4::mat4f initialMatrix;
	
	//can be nullptr = set screen framebuffer
	void setFramebuffer(std::shared_ptr<FrameBuffer> fb);
	
	virtual void clearFramebuffer() = 0;
	
	virtual bool isScissorEnabled()const = 0;
	
	virtual void setScissorEnabled(bool enabled) = 0;
	
	virtual r4::recti getScissorRect()const = 0;
	
	virtual void setScissorRect(r4::recti r) = 0;
	
	virtual r4::recti getViewport()const = 0;
	
	virtual void setViewport(r4::recti r) = 0;
	
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
