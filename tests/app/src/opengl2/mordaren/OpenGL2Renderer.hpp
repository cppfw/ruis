#pragma once

#include <memory>

#include <GL/glew.h>

#include <morda/render/renderer.hpp>

#include "OpenGL2Factory.hpp"

namespace mordaren{

class OpenGL2Renderer : public morda::renderer{
	GLuint defaultFramebuffer;
public:
	OpenGL2Renderer(std::unique_ptr<OpenGL2Factory> factory = std::make_unique<OpenGL2Factory>());
	
	OpenGL2Renderer(const OpenGL2Renderer& orig) = delete;
	OpenGL2Renderer& operator=(const OpenGL2Renderer& orig) = delete;
	
	void set_framebuffer_internal(morda::frame_buffer* fb)override;

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
