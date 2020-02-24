#include <utki/config.hpp>

#include "OpenGLES2Factory.hpp"

#include "OpenGLES2VertexBuffer.hpp"
#include "OpenGLES2VertexArray.hpp"

#include "OpenGLES2_util.hpp"
#include "OpenGLES2IndexBuffer.hpp"

#include "OpenGLES2Texture2D.hpp"
#include "OpenGLES2ShaderPosTex.hpp"
#include "OpenGLES2ShaderColor.hpp"
#include "OpenGLES2ShaderPosClr.hpp"
#include "OpenGLES2ShaderColorPosTex.hpp"
#include "OpenGLES2ShaderColorPosLum.hpp"
#include "OpenGLES2FrameBuffer.hpp"


#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif


using namespace mordaren;

OpenGLES2Factory::OpenGLES2Factory(){
	//TODO:
}


OpenGLES2Factory::~OpenGLES2Factory()noexcept{
	//TODO:
}



std::shared_ptr<morda::texture_2d> OpenGLES2Factory::create_texture_2d(morda::texture_2d::type type, r4::vec2ui dims, const utki::span<uint8_t> data){
	//TODO: turn these asserts to real checks with exceptions throwing
	ASSERT(data.size() % morda::texture_2d::bytes_per_pixel(type) == 0)
	ASSERT(data.size() % dims.x == 0)

	ASSERT(data.size() == 0 || data.size() / morda::texture_2d::bytes_per_pixel(type) / dims.x == dims.y)
	
	auto ret = std::make_shared<OpenGLES2Texture2D>(dims.to<float>());
	
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

std::shared_ptr<morda::VertexBuffer> OpenGLES2Factory::create_vertex_buffer(const utki::span<r4::vec4f> vertices){
	return std::make_shared<OpenGLES2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexBuffer> OpenGLES2Factory::create_vertex_buffer(const utki::span<r4::vec3f> vertices){
	return std::make_shared<OpenGLES2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexBuffer> OpenGLES2Factory::create_vertex_buffer(const utki::span<r4::vec2f> vertices){
	return std::make_shared<OpenGLES2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexBuffer> OpenGLES2Factory::create_vertex_buffer(const utki::span<float> vertices){
	return std::make_shared<OpenGLES2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexArray> OpenGLES2Factory::create_vertex_array(
		std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers,
		std::shared_ptr<morda::IndexBuffer> indices,
		morda::VertexArray::Mode_e mode
	)
{
	return std::make_shared<OpenGLES2VertexArray>(std::move(buffers), std::move(indices), mode);
}

std::shared_ptr<morda::IndexBuffer> OpenGLES2Factory::create_index_buffer(const utki::span<uint16_t> indices){
	return std::make_shared<OpenGLES2IndexBuffer>(indices);
}

std::unique_ptr<morda::render_factory::shaders> OpenGLES2Factory::create_shaders(){
	auto ret = std::make_unique<morda::render_factory::shaders>();
	ret->pos_tex = std::make_unique<OpenGLES2ShaderPosTex>();
	ret->color_pos = std::make_unique<OpenGLES2ShaderColor>();
	ret->pos_clr = std::make_unique<OpenGLES2ShaderPosClr>();
	ret->color_pos_tex = std::make_unique<OpenGLES2ShaderColorPosTex>();
	ret->color_pos_lum = std::make_unique<OpenGLES2ShaderColorPosLum>();
	return ret;
}

std::shared_ptr<morda::frame_buffer> OpenGLES2Factory::create_framebuffer(std::shared_ptr<morda::texture_2d> color){
	return std::make_shared<OpenGLES2FrameBuffer>(std::move(color));
}

