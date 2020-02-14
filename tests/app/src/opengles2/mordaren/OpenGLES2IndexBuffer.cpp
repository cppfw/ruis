#include <utki/config.hpp>

#include "OpenGLES2IndexBuffer.hpp"

#include "OpenGLES2_util.hpp"

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace mordaren;

OpenGLES2IndexBuffer::OpenGLES2IndexBuffer(const utki::span<std::uint16_t> indices) :
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
