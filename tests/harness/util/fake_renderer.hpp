#pragma once

#include <memory>

#include "../../../src/morda/render/renderer.hpp"

class fake_texture_2d : public morda::texture_2d{
public:
	fake_texture_2d() : morda::texture_2d(morda::vector2(13, 666)){}
};

class fake_frame_buffer : public morda::frame_buffer{
public:
	fake_frame_buffer() : morda::frame_buffer(utki::make_shared<fake_texture_2d>()){}
};

class fake_index_buffer : public morda::index_buffer{
public:
};

class fake_vertex_buffer : public morda::vertex_buffer{
public:
	fake_vertex_buffer() :
		morda::vertex_buffer(13)
	{}
};

class fake_vertex_array : public morda::vertex_array{
public:
	fake_vertex_array() :
		morda::vertex_array(
			{utki::make_shared<fake_vertex_buffer>()},
			utki::make_shared<fake_index_buffer>(),
			morda::vertex_array::mode::triangles
		)
	{}
};

class fake_factory : public morda::render_factory{
public:
	utki::shared_ref<morda::frame_buffer> create_framebuffer(
		const utki::shared_ref<morda::texture_2d>& color
	)override
	{
		return utki::make_shared<fake_frame_buffer>();
	}

	utki::shared_ref<morda::index_buffer> create_index_buffer(utki::span<const uint16_t> indices)override{
		return utki::make_shared<fake_index_buffer>();
	}

	std::unique_ptr<morda::render_factory::shaders> create_shaders() override{
		return nullptr;
	}

	utki::shared_ref<morda::texture_2d> create_texture_2d(const rasterimage::image_variant& imvar)override{
		return utki::make_shared<fake_texture_2d>();
	}

	utki::shared_ref<morda::vertex_array> create_vertex_array(
			std::vector<utki::shared_ref<const morda::vertex_buffer>>&& buffers,
			const utki::shared_ref<const morda::index_buffer>& indices,
			morda::vertex_array::mode rendering_mode
		)override
	{
		return utki::make_shared<fake_vertex_array>();
	}

	utki::shared_ref<morda::vertex_buffer> create_vertex_buffer(utki::span<const float> vertices)override{
		return utki::make_shared<fake_vertex_buffer>();
	}

	utki::shared_ref<morda::vertex_buffer> create_vertex_buffer(utki::span<const r4::vector2<float>> vertices)override{
		return utki::make_shared<fake_vertex_buffer>();
	}
	utki::shared_ref<morda::vertex_buffer> create_vertex_buffer(utki::span<const r4::vector3<float>> vertices)override{
		return utki::make_shared<fake_vertex_buffer>();
	}

	utki::shared_ref<morda::vertex_buffer> create_vertex_buffer(utki::span<const r4::vector4<float>> vertices)override{
		return utki::make_shared<fake_vertex_buffer>();
	}

};

class fake_renderer : public morda::renderer{
public:
	fake_renderer() :
			morda::renderer(std::make_unique<fake_factory>(), params())
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
	void set_framebuffer_internal(morda::frame_buffer* fb)override{}
	void set_scissor_enabled(bool enabled)override{}
	void set_scissor(r4::rectangle<int> r)override{}
	void set_viewport(r4::rectangle<int> r)override{}
};
