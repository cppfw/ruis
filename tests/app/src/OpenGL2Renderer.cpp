#include "OpenGL2Renderer.hpp"

#include "OpenGL2VertexBuffer.hpp"

#include <GL/glew.h>


inline static void AssertOpenGLNoError(){
#ifdef DEBUG
	GLenum error = glGetError();
	switch(error){
		case GL_NO_ERROR:
			return;
		case GL_INVALID_ENUM:
			ASSERT_INFO(false, "OpenGL error: GL_INVALID_ENUM")
			break;
		case GL_INVALID_VALUE:
			ASSERT_INFO(false, "OpenGL error: GL_INVALID_VALUE")
			break;
		case GL_INVALID_OPERATION:
			ASSERT_INFO(false, "OpenGL error: GL_INVALID_OPERATION")
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			ASSERT_INFO(false, "OpenGL error: GL_INVALID_FRAMEBUFFER_OPERATION")
			break;
		case GL_OUT_OF_MEMORY:
			ASSERT_INFO(false, "OpenGL error: GL_OUT_OF_MEMORY")
			break;
		default:
			ASSERT_INFO(false, "Unknown OpenGL error, code = " << int(error))
			break;
	}
#endif
}


OpenGL2ShaderPosTex::OpenGL2ShaderPosTex() :
		OpenGL2Shader(
				R"qwertyuiop(
						#ifndef GL_ES
						#	define highp
						#	define mediump
						#	define lowp
						#endif

						attribute highp vec4 pos;

						attribute highp vec2 texCoord0;

						uniform highp mat4 matrix;

						varying highp vec2 tc0;

						void main(void){
							gl_Position = matrix * pos;
							tc0 = texCoord0;
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
							gl_FragColor = texture2D(texture0, tc0);
						}
					)qwertyuiop"
			)
{
	this->posAttrib = this->getAttribute("pos");
	this->textureUniform = this->getUniform("texture0");
	this->texCoordAttrib = this->getAttribute("texCoord0");
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
	
	this->setVertexAttribArray(this->posAttrib, &*p.begin());
	this->setVertexAttribArray(this->texCoordAttrib, &*t.begin());
	this->renderElements(mode, i);
}



std::shared_ptr<morda::VertexBuffer> OpenGL2Renderer::createVertexBuffer(const utki::Buf<kolme::Vec2f> vertices){
	return utki::makeShared<OpenGL2VertexBuffer>(vertices);
}
