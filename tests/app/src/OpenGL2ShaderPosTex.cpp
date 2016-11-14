#include "OpenGL2ShaderPosTex.hpp"

#include "OpenGL2Texture2D.hpp"
#include "OpenGL2VertexBuffer.hpp"
#include "OpenGL2VertexArray.hpp"
#include "OpenGL2IndexBuffer.hpp"

#include "OpenGL2_util.hpp"

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
							gl_FragColor = texture2D(texture0, tc0);
						}
					)qwertyuiop"
			)
{
	this->textureUniform = this->getUniform("texture0");
	this->texCoordAttrib = 1;
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
	auto& vao = static_cast<const OpenGL2VertexArray&>(va);
	
	ASSERT(dynamic_cast<const OpenGL2IndexBuffer*>(&e))
	const OpenGL2IndexBuffer& ivbo = static_cast<const OpenGL2IndexBuffer&>(e);
	
	static_cast<const OpenGL2Texture2D&>(tex).bind(0);
	this->bind();
	
	this->setMatrix(m);
	
	glBindVertexArray(vao.arr);
	AssertOpenGLNoError();

	glDrawElements(modeMap[unsigned(mode)], ivbo.elementsCount, ivbo.elementType, nullptr);
	AssertOpenGLNoError();

	//TODO: remove this
	glBindVertexArray(0);
	AssertOpenGLNoError();
}

