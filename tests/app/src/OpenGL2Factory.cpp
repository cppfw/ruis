#include "OpenGL2Factory.hpp"

#include "OpenGL2VertexBuffer.hpp"
#include "OpenGL2VertexArray.hpp"

#include <GL/glew.h>

#include "OpenGL2_util.hpp"
#include "OpenGL2IndexBuffer.hpp"

#include "OpenGL2Texture2D.hpp"
#include "OpenGL2ShaderColorPos.hpp"




OpenGL2Factory::OpenGL2Factory(){
	//TODO:
}


OpenGL2Factory::~OpenGL2Factory()noexcept{
	//TODO:
}



std::shared_ptr<morda::Texture2D_n> OpenGL2Factory::createTexture2D(TexType_e type, kolme::Vec2ui dim, const utki::Buf<std::uint8_t>& data) {
	//TODO: turn these asserts to real checks with exceptions throwing
	ASSERT(data.size() % bytesPerPixel(type) == 0)
	ASSERT(data.size() % dim.x == 0)

	ASSERT(data.size() == 0 || data.size() / bytesPerPixel(type) / dim.x == dim.y)
	
	auto ret = utki::makeShared<OpenGL2Texture2D>(dim.to<float>());
	
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
	AssertOpenGLNoError();

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
	AssertOpenGLNoError();

	//NOTE: on OpenGL ES 2 it is necessary to set the filter parameters
	//      for every texture!!! Otherwise it may not work!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	AssertOpenGLNoError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	AssertOpenGLNoError();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	return ret;
}


std::shared_ptr<morda::VertexBuffer> OpenGL2Factory::createVertexBuffer(const utki::Buf<kolme::Vec3f> vertices){
	return utki::makeShared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexBuffer> OpenGL2Factory::createVertexBuffer(const utki::Buf<kolme::Vec2f> vertices){
	return utki::makeShared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexArray> OpenGL2Factory::createVertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, morda::VertexArray::Mode_e mode) {
	return utki::makeShared<OpenGL2VertexArray>(std::move(buffers), std::move(indices), mode);
}

std::shared_ptr<morda::IndexBuffer> OpenGL2Factory::createIndexBuffer(const utki::Buf<std::uint16_t> indices) {
	return utki::makeShared<OpenGL2IndexBuffer>(indices);
}

std::unique_ptr<morda::ShaderPosTex> OpenGL2Factory::createPosTexShader() {
	return utki::makeUnique<OpenGL2ShaderPosTex>();
}

std::unique_ptr<morda::ShaderColorPos> OpenGL2Factory::createColorPosShader() {
	return utki::makeUnique<OpenGL2ShaderColorPos>();
}

