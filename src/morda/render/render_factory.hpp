#pragma once

#include <vector>

#include <r4/vector2.hpp>

#include <utki/shared.hpp>
#include <utki/Unique.hpp>
#include <utki/Buf.hpp>

#include "Texture2D.hpp"

#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"

#include "ShaderTexture.hpp"
#include "coloring_shader.hpp"
#include "shader.hpp"
#include "coloring_texturing_shader.hpp"
#include "frame_buffer.hpp"

namespace morda{

class render_factory{
protected:
	render_factory(){}
	
public:
	virtual ~render_factory()noexcept{}	
	
	virtual std::shared_ptr<Texture2D> create_texture_2d(Texture2D::TexType_e type, r4::vec2ui dims, const utki::span<uint8_t> data) = 0;
	
	std::shared_ptr<Texture2D> create_texture_2d(r4::vec2ui dims, const utki::span<uint32_t> data);
	
	virtual std::shared_ptr<VertexBuffer> create_vertex_buffer(const utki::span<r4::vec4f> vertices) = 0;
	
	virtual std::shared_ptr<VertexBuffer> create_vertex_buffer(const utki::span<r4::vec3f> vertices) = 0;
	
	virtual std::shared_ptr<VertexBuffer> create_vertex_buffer(const utki::span<r4::vec2f> vertices) = 0;
	
	virtual std::shared_ptr<VertexBuffer> create_vertex_buffer(const utki::span<float> vertices) = 0;
	
	virtual std::shared_ptr<IndexBuffer> create_index_buffer(const utki::span<uint16_t> indices) = 0;
	
	virtual std::shared_ptr<VertexArray> create_vertex_array(
			std::vector< std::shared_ptr<morda::VertexBuffer> >&& buffers,
			std::shared_ptr<morda::IndexBuffer> indices,
			VertexArray::Mode_e mode
		) = 0;
	
	struct shaders{
		std::unique_ptr<ShaderTexture> pos_tex;
		std::unique_ptr<coloring_shader> color_pos;
		std::unique_ptr<coloring_shader> color_pos_lum;
		std::unique_ptr<shader> pos_clr;
		std::unique_ptr<coloring_texturing_shader> color_pos_tex;
	};
	
	virtual std::unique_ptr<shaders> create_shaders() = 0;
	
	virtual std::shared_ptr<frame_buffer> create_framebuffer(std::shared_ptr<Texture2D> color) = 0;
};

}
