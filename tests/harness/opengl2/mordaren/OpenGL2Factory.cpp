#include "OpenGL2Factory.hpp"

#include "OpenGL2VertexBuffer.hpp"
#include "OpenGL2VertexArray.hpp"

#include "OpenGL2_util.hpp"
#include "OpenGL2IndexBuffer.hpp"

#include "OpenGL2Texture2D.hpp"
#include "OpenGL2ShaderTexture.hpp"
#include "OpenGL2ShaderColor.hpp"
#include "OpenGL2ShaderPosClr.hpp"
#include "OpenGL2ShaderColorPosTex.hpp"
#include "OpenGL2ShaderColorPosLum.hpp"
#include "OpenGL2FrameBuffer.hpp"

#include <GL/glew.h>

using namespace mordaren;

OpenGL2Factory::OpenGL2Factory(){}

OpenGL2Factory::~OpenGL2Factory()noexcept{}

std::shared_ptr<morda::texture_2d> OpenGL2Factory::create_texture_2d(morda::texture_2d::type type, r4::vec2ui dims, utki::span<const uint8_t> data){
	//TODO: turn these asserts to real checks with exceptions throwing
	ASSERT(data.size() % morda::texture_2d::bytes_per_pixel(type) == 0)
	ASSERT(data.size() % dims.x == 0)

	ASSERT(data.size() == 0 || data.size() / morda::texture_2d::bytes_per_pixel(type) / dims.x == dims.y)
	
	auto ret = std::make_shared<OpenGL2Texture2D>(dims.to<float>());
	
	//TODO: save previous bind and restore it after?
	ret->bind(0);
	
	GLint internalFormat;
	switch(type){
		default:
			ASSERT(false)
		case decltype(type)::grey:
			internalFormat = GL_LUMINANCE;
			break;
		case decltype(type)::grey_alpha:
			internalFormat = GL_LUMINANCE_ALPHA;
			break;
		case decltype(type)::rgb:
			internalFormat = GL_RGB;
			break;
		case decltype(type)::rgba:
			internalFormat = GL_RGBA;
			break;
	}

	// we will be passing pixels to OpenGL which are 1-byte aligned.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	assertOpenGLNoError();

	glTexImage2D(
			GL_TEXTURE_2D,
			0, // 0th level, no mipmaps
			internalFormat, // internal format
			dims.x,
			dims.y,
			0, // border, should be 0!
			internalFormat, // format of the texel data
			GL_UNSIGNED_BYTE,
			data.size() == 0 ? nullptr : &*data.begin()
		);
	assertOpenGLNoError();

	// NOTE: on OpenGL ES 2 it is necessary to set the filter parameters
	//       for every texture!!! Otherwise it may not work!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	assertOpenGLNoError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	assertOpenGLNoError();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	return ret;
}

std::shared_ptr<morda::vertex_buffer> OpenGL2Factory::create_vertex_buffer(utki::span<const r4::vec4f> vertices){
	return std::make_shared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::vertex_buffer> OpenGL2Factory::create_vertex_buffer(utki::span<const r4::vec3f> vertices){
	return std::make_shared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::vertex_buffer> OpenGL2Factory::create_vertex_buffer(utki::span<const r4::vec2f> vertices){
	return std::make_shared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::vertex_buffer> OpenGL2Factory::create_vertex_buffer(utki::span<const float> vertices){
	return std::make_shared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::vertex_array> OpenGL2Factory::create_vertex_array(
		std::vector<std::shared_ptr<morda::vertex_buffer>>&& buffers,
		std::shared_ptr<morda::index_buffer> indices,
		morda::vertex_array::mode mode
	)
{
	return std::make_shared<OpenGL2VertexArray>(std::move(buffers), std::move(indices), mode);
}

std::shared_ptr<morda::index_buffer> OpenGL2Factory::create_index_buffer(utki::span<const uint16_t> indices){
	return std::make_shared<OpenGL2IndexBuffer>(indices);
}

std::unique_ptr<morda::render_factory::shaders> OpenGL2Factory::create_shaders(){
	auto ret = std::make_unique<morda::render_factory::shaders>();
	ret->pos_tex = std::make_unique<OpenGL2ShaderTexture>();
	ret->color_pos = std::make_unique<OpenGL2ShaderColor>();
	ret->pos_clr = std::make_unique<OpenGL2ShaderPosClr>();
	ret->color_pos_tex = std::make_unique<OpenGL2ShaderColorPosTex>();
	ret->color_pos_lum = std::make_unique<OpenGL2ShaderColorPosLum>();
	return ret;
}

std::shared_ptr<morda::frame_buffer> OpenGL2Factory::create_framebuffer(std::shared_ptr<morda::texture_2d> color){
	return std::make_shared<OpenGL2FrameBuffer>(std::move(color));
}
