#pragma once

#include <memory>

#include <ruis/render/renderer.hpp>

class null_texture_2d : public ruis::texture_2d{
public:
	null_texture_2d() : ruis::texture_2d(ruis::vector2(13, 666)){}
};

class null_frame_buffer : public ruis::frame_buffer{
public:
	null_frame_buffer() : ruis::frame_buffer(utki::make_shared<null_texture_2d>()){}
};

class null_index_buffer : public ruis::index_buffer{
public:
};

class null_vertex_buffer : public ruis::vertex_buffer{
public:
	null_vertex_buffer() :
		ruis::vertex_buffer(13)
	{}
};

class null_vertex_array : public ruis::vertex_array{
public:
	null_vertex_array() :
		ruis::vertex_array(
			{utki::make_shared<null_vertex_buffer>()},
			utki::make_shared<null_index_buffer>(),
			ruis::vertex_array::mode::triangles
		)
	{}
};

class null_factory : public ruis::render_factory{
public:
	utki::shared_ref<ruis::frame_buffer> create_framebuffer(
		const utki::shared_ref<ruis::texture_2d>& color
	)override
	{
		return utki::make_shared<null_frame_buffer>();
	}

	utki::shared_ref<ruis::index_buffer> create_index_buffer(utki::span<const uint16_t> indices)override{
		return utki::make_shared<null_index_buffer>();
	}

	std::unique_ptr<ruis::render_factory::shaders> create_shaders() override{
		return nullptr;
	}

	utki::shared_ref<ruis::texture_2d> create_texture_2d(
		rasterimage::format format,
		rasterimage::dimensioned::dimensions_type dims
	)override
	{
		return utki::make_shared<null_texture_2d>();
	}

	utki::shared_ref<ruis::texture_2d> create_texture_2d(const rasterimage::image_variant& imvar)override{
		return utki::make_shared<null_texture_2d>();
	}

	utki::shared_ref<ruis::vertex_array> create_vertex_array(
			std::vector<utki::shared_ref<const ruis::vertex_buffer>> buffers,
			const utki::shared_ref<const ruis::index_buffer>& indices,
			ruis::vertex_array::mode rendering_mode
		)override
	{
		return utki::make_shared<null_vertex_array>();
	}

	utki::shared_ref<ruis::vertex_buffer> create_vertex_buffer(utki::span<const float> vertices)override{
		return utki::make_shared<null_vertex_buffer>();
	}

	utki::shared_ref<ruis::vertex_buffer> create_vertex_buffer(utki::span<const r4::vector2<float>> vertices)override{
		return utki::make_shared<null_vertex_buffer>();
	}
	utki::shared_ref<ruis::vertex_buffer> create_vertex_buffer(utki::span<const r4::vector3<float>> vertices)override{
		return utki::make_shared<null_vertex_buffer>();
	}

	utki::shared_ref<ruis::vertex_buffer> create_vertex_buffer(utki::span<const r4::vector4<float>> vertices)override{
		return utki::make_shared<null_vertex_buffer>();
	}

};

class null_renderer : public ruis::renderer{
public:
	null_renderer() :
			ruis::renderer(std::make_unique<null_factory>(), params())
	{}

	void clear_framebuffer()override{}
	r4::rectangle<int> get_scissor()const override{
		return {0, 0};
	}
	r4::rectangle<int> get_viewport()const override{
		return {0, 0};
	}
	bool is_scissor_enabled()const override{
		return false;
	}
	void set_blend_enabled(bool enable)override{}
	void set_blend_func(blend_factor src_color, blend_factor dst_color, blend_factor src_alpha, blend_factor dst_alpha)override{}
	void set_framebuffer_internal(ruis::frame_buffer* fb)override{}
	void set_scissor_enabled(bool enabled)override{}
	void set_scissor(r4::rectangle<int> r)override{}
	void set_viewport(r4::rectangle<int> r)override{}
};
