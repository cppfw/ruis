#include "render_factory.hpp"

#include "OpenGL2VertexBuffer.hpp"
#include "OpenGL2VertexArray.hpp"

#include <GL/glew.h>

#include "OpenGL2_util.hpp"
#include "index_buffer.hpp"

#include "OpenGL2Texture2D.hpp"
#include "OpenGL2ShaderColorPos.hpp"
#include "shader_pos_clr.hpp"
#include "shader_color_pos_tex.hpp"
#include "frame_buffer.hpp"

render_factory::render_factory(){}

render_factory::~render_factory()noexcept{}

std::shared_ptr<morda::Texture2D> render_factory::createTexture2D(morda::Texture2D::TexType_e type, kolme::Vec2ui dim, utki::span<const std::uint8_t>& data) {
	//TODO: turn these asserts to real checks with exceptions throwing
	ASSERT(data.size() % morda::Texture2D::bytesPerPixel(type) == 0)
	ASSERT(data.size() % dim.x == 0)

	ASSERT(data.size() == 0 || data.size() / morda::Texture2D::bytesPerPixel(type) / dim.x == dim.y)
	
	auto ret = std::make_shared<OpenGL2Texture2D>(dim.to<float>());
	
	//TODO: save previous bind and restore it after?
	ret->bind(0);
	
	GLint internalFormat;
	switch(type){
		default:
			ASSERT(false)
		case decltype(type)::GREY:
			internalFormat = GL_LUMINANCE;
			break;
		case decltype(type)::GREYA:
			internalFormat = GL_LUMINANCE_ALPHA;
			break;
		case decltype(type)::RGB:
			internalFormat = GL_RGB;
			break;
		case decltype(type)::RGBA:
			internalFormat = GL_RGBA;
			break;
	}

	//we will be passing pixels to OpenGL which are 1-byte aligned.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	assertOpenGLNoError();

	glTexImage2D(
			GL_TEXTURE_2D,
			0,//0th level, no mipmaps
			internalFormat, //internal format
			dim.x,
			dim.y,
			0,//border, should be 0!
			internalFormat, //format of the texel data
			GL_UNSIGNED_BYTE,
			data.size() == 0 ? nullptr : &*data.begin()
		);
	assertOpenGLNoError();

	//NOTE: on OpenGL ES 2 it is necessary to set the filter parameters
	//      for every texture!!! Otherwise it may not work!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	assertOpenGLNoError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	assertOpenGLNoError();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	return ret;
}

std::shared_ptr<morda::VertexBuffer> render_factory::createVertexBuffer(utki::span<const kolme::Vec4f> vertices){
	return std::make_shared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexBuffer> render_factory::createVertexBuffer(utki::span<const kolme::Vec3f> vertices){
	return std::make_shared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexBuffer> render_factory::createVertexBuffer(utki::span<const kolme::Vec2f> vertices){
	return std::make_shared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexArray> render_factory::createVertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, morda::VertexArray::Mode_e mode) {
	return std::make_shared<OpenGL2VertexArray>(std::move(buffers), std::move(indices), mode);
}

std::shared_ptr<morda::IndexBuffer> render_factory::createIndexBuffer(utki::span<const std::uint16_t> indices) {
	return std::make_shared<index_buffer>(indices);
}

std::unique_ptr<morda::Factory::Shaders> render_factory::createShaders() {
	auto ret = std::make_unique<morda::Factory::Shaders>();
	ret->posTex = std::make_unique<OpenGL2ShaderPosTex>();
	ret->colorPos = std::make_unique<OpenGL2ShaderColorPos>();
	ret->posClr = std::make_unique<shader_pos_clr>();
	ret->colorPosTex = std::make_unique<shader_color_pos_tex>();
	return ret;
}

std::shared_ptr<morda::FrameBuffer> render_factory::createFramebuffer(std::shared_ptr<morda::Texture2D> color) {
	return std::make_shared<frame_buffer>(std::move(color));
}

