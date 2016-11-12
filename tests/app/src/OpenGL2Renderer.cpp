#include "OpenGL2Renderer.hpp"

#include "OpenGL2VertexBuffer.hpp"
#include "OpenGL2VertexArray.hpp"

#include <GL/glew.h>

#include "OpenGL2_util.hpp"
#include "OpenGL2IndexBuffer.hpp"


OpenGL2ShaderPosTex::OpenGL2ShaderPosTex() :
		OpenGL2Shader(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						attribute highp vec4 a0; //position

						attribute highp vec2 a1; //texture coordinates

						uniform highp mat4 matrix;

						varying highp vec2 tc0;

						void main(void){
							gl_Position = matrix * a0;
							tc0 = a1;
						}
					)qwertyuiop",
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif
		
						uniform sampler2D texture0;
		
						varying highp vec2 tc0;
		
						void main(void){
							gl_FragColor = texture2D(texture0, tc0) * 0.1 + vec4(0.9, 0.9, 0, 0.9);
						}
					)qwertyuiop"
			)
{
	this->textureUniform = this->getUniform("texture0");
	this->texCoordAttrib = 1;
}



OpenGL2Renderer::OpenGL2Renderer() :
		Renderer(this->sPosTex)
{
	//TODO:
}


OpenGL2Renderer::~OpenGL2Renderer()noexcept{
	//TODO:
}


namespace{
struct OpenGL2Texture2D : public morda::Texture2D_n{
	GLuint tex;
	
	OpenGL2Texture2D(){
		glGenTextures(1, &this->tex);
		AssertOpenGLNoError();
		ASSERT(this->tex != 0)
	}
	
	virtual ~OpenGL2Texture2D()noexcept{
		glDeleteTextures(1, &this->tex);
	}
	
	void bind(unsigned unitNum)const{
		glActiveTexture(GL_TEXTURE0 + unitNum);
		AssertOpenGLNoError();
		glBindTexture(GL_TEXTURE_2D, this->tex);
		AssertOpenGLNoError();
	}
};
}


std::shared_ptr<morda::Texture2D_n> OpenGL2Renderer::createTexture2D(TexType_e type, kolme::Vec2ui dim, const utki::Buf<std::uint8_t>& data) {
	//TODO: turn these asserts to real checks with exceptions throwing
	ASSERT(data.size() % bytesPerPixel(type) == 0)
	ASSERT(data.size() % dim.x == 0)

	ASSERT(data.size() == 0 || data.size() / bytesPerPixel(type) / dim.x == dim.y)
	
	auto ret = utki::makeShared<OpenGL2Texture2D>();
	
	//TODO: save previous bing and restore it after?
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

void OpenGL2ShaderPosTex::render(const kolme::Matr4f& m, const morda::Texture2D_n& tex, const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec2f> t, Mode_e mode) {
	static_cast<const OpenGL2Texture2D&>(tex).bind(0);
	
	this->bind();
	
	//TODO:
	
}

void OpenGL2ShaderPosTex::render(const kolme::Matr4f& m, const morda::Texture2D_n& tex, const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec2f> t, Mode_e mode) {
	static_cast<const OpenGL2Texture2D&>(tex).bind(0);
	this->bind();
	
	this->setMatrix(m);
	
	this->setPosAttribArray(&*p.begin());
	this->setVertexAttribArray(this->texCoordAttrib, &*t.begin());
	this->renderElements(mode, i);
}

namespace{
GLenum modeMap[] = {
	GL_TRIANGLES,			//TRIANGLES
	GL_TRIANGLE_FAN,		//TRIANGLE_FAN
	GL_LINE_LOOP,			//LINE_LOOP
	GL_TRIANGLE_STRIP		//TRIANGLE_STRIP
};
}

void OpenGL2ShaderPosTex::render(const kolme::Matr4f& m, const morda::Texture2D_n& tex, const morda::VertexBuffer& p, const morda::IndexBuffer& e, Mode_e mode = Mode_e::TRIANGLE_FAN){
	ASSERT(dynamic_cast<const OpenGL2VertexBuffer*>(&p))
	const OpenGL2VertexBuffer& pb = static_cast<const OpenGL2VertexBuffer&>(p);
	
	ASSERT(dynamic_cast<const OpenGL2IndexBuffer*>(&e))
	const OpenGL2IndexBuffer& eb = static_cast<const OpenGL2IndexBuffer&>(e);
	
	static_cast<const OpenGL2Texture2D&>(tex).bind(0);
	this->bind();
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, pb.buffer);
	AssertOpenGLNoError();
	
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, eb.buffer);
	AssertOpenGLNoError();

//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(3, GL_FLOAT, 0, 0);

	glDrawElements(modeMap[unsigned(mode)], eb.elementsCount, eb.elementType, 0);

//	glDisableClientState(GL_VERTEX_ARRAY);

	//TODO: remove this
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	AssertOpenGLNoError();
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	AssertOpenGLNoError();
}


void OpenGL2ShaderPosTex::render(const kolme::Matr4f& m, const morda::Texture2D_n& tex, const morda::VertexArray& va, const morda::IndexBuffer& e, Mode_e mode = Mode_e::TRIANGLE_FAN){
	ASSERT(dynamic_cast<const OpenGL2VertexArray*>(&va))
	auto& a = static_cast<const OpenGL2VertexArray&>(va);
	
	ASSERT(dynamic_cast<const OpenGL2IndexBuffer*>(&e))
	const OpenGL2IndexBuffer& eb = static_cast<const OpenGL2IndexBuffer&>(e);
	
	static_cast<const OpenGL2Texture2D&>(tex).bind(0);
	this->bind();
	
	glBindVertexArray(a.arr);
	AssertOpenGLNoError();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb.buffer);
	AssertOpenGLNoError();

	for(unsigned i = 0; i != a.buffers.size(); ++i){
		glEnableVertexAttribArray(i);
		AssertOpenGLNoError();
	}

	glDrawElements(modeMap[unsigned(mode)], eb.elementsCount, eb.elementType, 0);

	for(unsigned i = 0; i != a.buffers.size(); ++i){
		glDisableVertexAttribArray(i);
		AssertOpenGLNoError();
	}

	//TODO: remove this
	glBindVertexArray(0);
	AssertOpenGLNoError();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	AssertOpenGLNoError();
}


std::shared_ptr<morda::VertexBuffer> OpenGL2Renderer::createVertexBuffer(const utki::Buf<kolme::Vec3f> vertices){
	return utki::makeShared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexBuffer> OpenGL2Renderer::createVertexBuffer(const utki::Buf<kolme::Vec2f> vertices){
	return utki::makeShared<OpenGL2VertexBuffer>(vertices);
}

std::shared_ptr<morda::VertexArray> OpenGL2Renderer::createVertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers) {
	return utki::makeShared<OpenGL2VertexArray>(std::move(buffers));
}

std::shared_ptr<morda::IndexBuffer> OpenGL2Renderer::createIndexBuffer(const utki::Buf<std::uint16_t> indices) {
	return utki::makeShared<OpenGL2IndexBuffer>(indices);
}
