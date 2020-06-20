#include "vertex_buffer.hpp"

#include "OpenGL2_util.hpp"



void vertex_buffer::init(GLsizeiptr size, const GLvoid* data) {
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	assertOpenGLNoError();
	
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	assertOpenGLNoError();
}


vertex_buffer::vertex_buffer(utki::span<const kolme::Vec4f> vertices) :
		morda::VertexBuffer(vertices.size()),
		numComponents(4),
		type(GL_FLOAT)
{
	this->init(vertices.size_bytes(), &*vertices.begin());
}

vertex_buffer::vertex_buffer(utki::span<const kolme::Vec3f> vertices) :
		morda::VertexBuffer(vertices.size()),
		numComponents(3),
		type(GL_FLOAT)
{
	this->init(vertices.size_bytes(), &*vertices.begin());
}

vertex_buffer::vertex_buffer(utki::span<const kolme::Vec2f> vertices) :
		morda::VertexBuffer(vertices.size()),
		numComponents(2),
		type(GL_FLOAT)
{
	this->init(vertices.size_bytes(), &*vertices.begin());
}
