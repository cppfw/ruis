#pragma once

#include "RenderFactory.hpp"

namespace morda{

class renderer : virtual public utki::shared{
public:
	const std::unique_ptr<RenderFactory> factory;
	
	const std::unique_ptr<RenderFactory::Shaders> shader;
	
public:
	const std::shared_ptr<VertexBuffer> quad_01_vbo;
	const std::shared_ptr<IndexBuffer> quad_indices;

	const std::shared_ptr<VertexArray> pos_quad_01_vao;
	
	const std::shared_ptr<VertexArray> pos_tex_quad_01_vao;
	
protected:
	struct params{
		unsigned max_texture_size = 2048;
		r4::mat4f initial_matrix = r4::mat4f().identity();
	};
	
	renderer(std::unique_ptr<RenderFactory> factory, const params& params);
	
	renderer(const renderer&) = delete;
	renderer& operator=(const renderer&) = delete;
	
private:
	std::shared_ptr<FrameBuffer> curFB;
public:
	const unsigned max_texture_size;
	
	/**
	 * @brief Initial matrix to use for rendering.
	 * This is the matrix which makes screen edges to be: left = -1, right = 1, top = 1, bottom = -1.
	 */
	const r4::mat4f initial_matrix;
	
	/**
	 * @brief Set current framebuffer.
	 * @param fb - framebuffer to set as the current one. If 'nullptr' then screen buffer is set as current frame buffer.
	 */
	void set_framebuffer(std::shared_ptr<FrameBuffer> fb);
	
	virtual void clear_framebuffer() = 0;
	
	virtual bool is_scissor_enabled()const = 0;
	
	virtual void set_scissor_enabled(bool enabled) = 0;
	
	virtual r4::recti get_scissor()const = 0;
	
	virtual void set_scissor(r4::recti r) = 0;
	
	virtual r4::recti get_viewport()const = 0;
	
	virtual void set_viewport(r4::recti r) = 0;
	
	virtual void set_blend_enabled(bool enable) = 0;
	
	/**
	 * @brief Blending factor type.
	 * Enumeration defines possible blending factor types.
	 */
	enum class blend_factor{
		// WARNING: do not change order
		
		zero,
		one,
		src_color,
		one_minus_src_color,
		dst_color,
		one_minus_dst_color,
		src_alpha,
		one_minus_src_alpha,
		dst_alpha,
		one_minus_dst_alpha,
		constant_color,
		one_minus_constant_color,
		constant_alpha,
		one_minus_constant_alpha,
		src_alpha_saturate
	};
	
	virtual void set_blend_func(blend_factor src_color, blend_factor dst_color, blend_factor src_alpha, blend_factor dst_alpha) = 0;
	
protected:
	virtual void set_framebuffer_internal(FrameBuffer* fb) = 0;
};

}
