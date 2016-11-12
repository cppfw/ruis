#include "OpenGL2VertexBuffer.hpp"

#include "OpenGL2_util.hpp"


OpenGL2VertexBuffer::OpenGL2VertexBuffer(const utki::Buf<kolme::Vec3f> vertices) :
		morda::VertexBuffer(vertices.size()),
		numComponents(3),
		type(GL_FLOAT)
{
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	AssertOpenGLNoError();
	
	glBufferData(GL_ARRAY_BUFFER, vertices.sizeInBytes(), &*vertices.begin(), GL_STATIC_DRAW);
	AssertOpenGLNoError();
	
	//TODO: remove this
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	AssertOpenGLNoError();
}

OpenGL2VertexBuffer::OpenGL2VertexBuffer(const utki::Buf<kolme::Vec2f> vertices) :
		morda::VertexBuffer(vertices.size()),
		numComponents(2),
		type(GL_FLOAT)
{
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	AssertOpenGLNoError();
	
	glBufferData(GL_ARRAY_BUFFER, vertices.sizeInBytes(), &*vertices.begin(), GL_STATIC_DRAW);
	AssertOpenGLNoError();
	
	//TODO: remove this
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	AssertOpenGLNoError();
}
