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


namespace{
GLenum modeMap[] = {
	GL_TRIANGLES,			//TRIANGLES
	GL_TRIANGLE_FAN,		//TRIANGLE_FAN
	GL_LINE_LOOP,			//LINE_LOOP
	GL_TRIANGLE_STRIP		//TRIANGLE_STRIP
};
}


void OpenGL2ShaderPosTex::render(const kolme::Matr4f& m, const morda::Texture2D_n& tex, const morda::VertexArray& va){
	ASSERT(dynamic_cast<const OpenGL2VertexArray*>(&va))
	auto& vao = static_cast<const OpenGL2VertexArray&>(va);
	
	ASSERT(dynamic_cast<const OpenGL2IndexBuffer*>(va.indices.operator ->()))
	const OpenGL2IndexBuffer& ivbo = static_cast<const OpenGL2IndexBuffer&>(*va.indices);
	
	static_cast<const OpenGL2Texture2D&>(tex).bind(0);
	this->bind();
	
	this->setMatrix(m);
	
	glBindVertexArray(vao.arr);
	AssertOpenGLNoError();

//	TRACE(<< "ivbo.elementsCount = " << ivbo.elementsCount << " ivbo.elementType = " << ivbo.elementType << std::endl)
	
	glDrawElements(modeMap[unsigned(va.mode)], ivbo.elementsCount, ivbo.elementType, nullptr);
	AssertOpenGLNoError();

	//TODO: remove this
	glBindVertexArray(0);
	AssertOpenGLNoError();
}

