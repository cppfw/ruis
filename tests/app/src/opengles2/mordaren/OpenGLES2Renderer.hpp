#pragma once

#include <utki/config.hpp>

#include <morda/render/renderer.hpp>

#include "OpenGLES2Factory.hpp"

namespace mordaren{

class OpenGLES2Renderer : public morda::renderer{
	bool defaultFramebufferInitialized = false;
	std::uint32_t defaultFramebuffer; // NOTE: GLuint is fixed 32bit type, according to OpenGL specs, so use std::uint32_t.
public:
	OpenGLES2Renderer(std::unique_ptr<OpenGLES2Factory> factory = utki::makeUnique<OpenGLES2Factory>());
	
	OpenGLES2Renderer(const OpenGLES2Renderer& orig) = delete;
	OpenGLES2Renderer& operator=(const OpenGLES2Renderer& orig) = delete;
	
	void set_framebuffer_internal(morda::FrameBuffer* fb)override;

	void clear_framebuffer()override;
	
	bool is_scissor_enabled()const override;
	
	void set_scissor_enabled(bool enabled)override;
	
	r4::recti get_scissor()const override;
	
	void set_scissor(r4::recti r)override;

	r4::recti get_viewport()const override;
	
	void set_viewport(r4::recti r)override;
	
	void set_blend_enabled(bool enable)override;

	void set_blend_func(blend_factor src_color, blend_factor dst_color, blend_factor src_alpha, blend_factor dst_alpha)override;

};

}
