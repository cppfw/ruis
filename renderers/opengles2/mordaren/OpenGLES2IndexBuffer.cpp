#include "OpenGLES2IndexBuffer.hpp"

#include "OpenGLES2_util.hpp"

#include <GLES2/gl2.h>

using namespace mordaren;

OpenGLES2IndexBuffer::OpenGLES2IndexBuffer(const utki::Buf<std::uint16_t> indices) :
		elementType(GL_UNSIGNED_SHORT),
		elementsCount(GLsizei(indices.size()))
{	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer);
	assertOpenGLNoError();
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.sizeInBytes(), &*indices.begin(), GL_STATIC_DRAW);
	assertOpenGLNoError();
	
	//TODO: remove this
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	assertOpenGLNoError();
}
