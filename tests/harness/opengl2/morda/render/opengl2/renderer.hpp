#pragma once

#include <memory>

#include <GL/glew.h>

#include <morda/render/renderer.hpp>

#include "factory.hpp"

namespace morda{ namespace render_opengl2{

class renderer : public morda::renderer{
	GLuint defaultFramebuffer;
public:
	renderer(std::unique_ptr<render_factory> factory = std::make_unique<render_factory>());
	
	renderer(const renderer& orig) = delete;
	renderer& operator=(const renderer& orig) = delete;
	
	void set_framebuffer_internal(morda::frame_buffer* fb)override;

	void clear_framebuffer()override;
	
	bool is_scissor_enabled()const override;
	
	void set_scissor_enabled(bool enabled)override;
	
	r4::rectangle<int> get_scissor()const override;
	
	void set_scissor(r4::rectangle<int> r)override;

	r4::rectangle<int> get_viewport()const override;
	
	void set_viewport(r4::rectangle<int> r)override;
	
	void set_blend_enabled(bool enable)override;

	void set_blend_func(blend_factor src_color, blend_factor dst_color, blend_factor src_alpha, blend_factor dst_alpha)override;

};

}}
