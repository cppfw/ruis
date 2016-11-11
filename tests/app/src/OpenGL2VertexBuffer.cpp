#include "OpenGL2VertexBuffer.hpp"

#include "OpenGL2_util.hpp"


OpenGL2VertexBuffer::OpenGL2VertexBuffer(const utki::Buf<kolme::Vec3f> vertices) :
		morda::VertexBuffer(vertices.size())
{
	glGenBuffers(1, &this->buffer);
	AssertOpenGLNoError();
	
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	AssertOpenGLNoError();
	
	glBufferData(GL_ARRAY_BUFFER, vertices.sizeInBytes(), &*vertices.begin(), GL_STATIC_DRAW);
	AssertOpenGLNoError();
	
	//TODO: remove this
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	AssertOpenGLNoError();
}



OpenGL2VertexBuffer::~OpenGL2VertexBuffer()noexcept{
	glDeleteBuffers(1, &this->buffer);
	AssertOpenGLNoError();
}
