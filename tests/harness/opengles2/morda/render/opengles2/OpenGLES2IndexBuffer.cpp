#include <utki/config.hpp>

#include "OpenGLES2IndexBuffer.hpp"

#include "OpenGLES2_util.hpp"

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace morda;

OpenGLES2IndexBuffer::OpenGLES2IndexBuffer(utki::span<const std::uint16_t> indices) :
		elementType(GL_UNSIGNED_SHORT),
		elementsCount(GLsizei(indices.size()))
{	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer);
	assertOpenGLNoError();
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), &*indices.begin(), GL_STATIC_DRAW);
	assertOpenGLNoError();
}
