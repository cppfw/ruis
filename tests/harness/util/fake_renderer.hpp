#pragma once

#include <memory>

#include "../../../src/morda/morda/render/renderer.hpp"

class fake_texture_2d : public morda::texture_2d{
public:
	fake_texture_2d() : morda::texture_2d(morda::vector2(13, 666)){}
};

class fake_factory : public morda::render_factory{
public:
	std::shared_ptr<morda::frame_buffer> create_framebuffer(std::shared_ptr<morda::texture_2d> color)override{
		return nullptr;
	}

	std::shared_ptr<morda::index_buffer> create_index_buffer(utki::span<const uint16_t> indices)override{
		return nullptr;
	}

	std::unique_ptr<morda::render_factory::shaders> create_shaders() override{
		return nullptr;
	}

	std::shared_ptr<morda::texture_2d> create_texture_2d(morda::texture_2d::type type, r4::vector2<unsigned> dims, utki::span<const uint8_t> data)override{
		return std::make_shared<fake_texture_2d>();
	}

	std::shared_ptr<morda::vertex_array> create_vertex_array(
			std::vector<std::shared_ptr<morda::vertex_buffer>>&& buffers,
			std::shared_ptr<morda::index_buffer> indices,
			morda::vertex_array::mode rendering_mode
		)override
	{
		return nullptr;
	}

	std::shared_ptr<morda::vertex_buffer> create_vertex_buffer(utki::span<const float> vertices)override{
		return nullptr;
	}

	std::shared_ptr<morda::vertex_buffer> create_vertex_buffer(utki::span<const r4::vector2<float>> vertices)override{
		return nullptr;
	}
	std::shared_ptr<morda::vertex_buffer> create_vertex_buffer(utki::span<const r4::vector3<float>> vertices)override{
		return nullptr;
	}

	std::shared_ptr<morda::vertex_buffer> create_vertex_buffer(utki::span<const r4::vector4<float>> vertices)override{
		return nullptr;
	}

};

class fake_renderer : public morda::renderer{
public:
	fake_renderer() :
			morda::renderer(std::make_unique<fake_factory>(), params())
	{}

	void clear_framebuffer()override{}
	r4::rectangle<int> get_scissor()const override{
		return r4::rectangle<int>(0, 0);
	}
	r4::rectangle<int> get_viewport()const override{
		return r4::rectangle<int>(0, 0);
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
