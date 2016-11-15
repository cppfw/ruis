#include "OpenGL2VertexBuffer.hpp"

#include "OpenGL2_util.hpp"



void OpenGL2VertexBuffer::init(GLsizeiptr size, const GLvoid* data) {
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	AssertOpenGLNoError();
	
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	AssertOpenGLNoError();
	
	//TODO: remove this
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	AssertOpenGLNoError();
}


OpenGL2VertexBuffer::OpenGL2VertexBuffer(const utki::Buf<kolme::Vec4f> vertices) :
		morda::VertexBuffer(vertices.size()),
		numComponents(4),
		type(GL_FLOAT)
{
	this->init(vertices.sizeInBytes(), &*vertices.begin());
}

OpenGL2VertexBuffer::OpenGL2VertexBuffer(const utki::Buf<kolme::Vec3f> vertices) :
		morda::VertexBuffer(vertices.size()),
		numComponents(3),
		type(GL_FLOAT)
{
	this->init(vertices.sizeInBytes(), &*vertices.begin());
}

OpenGL2VertexBuffer::OpenGL2VertexBuffer(const utki::Buf<kolme::Vec2f> vertices) :
		morda::VertexBuffer(vertices.size()),
		numComponents(2),
		type(GL_FLOAT)
{
	this->init(vertices.sizeInBytes(), &*vertices.begin());
}
