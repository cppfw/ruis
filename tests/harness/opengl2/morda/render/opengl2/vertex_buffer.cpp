#include "vertex_buffer.hpp"

#include "util.hpp"

using namespace morda::render_opengl2;

void vertex_buffer::init(GLsizeiptr size, const GLvoid* data) {
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	assertOpenGLNoError();
	
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	assertOpenGLNoError();
}


vertex_buffer::vertex_buffer(utki::span<const r4::vector4<float>> vertices) :
		morda::vertex_buffer(vertices.size()),
		numComponents(4),
		type(GL_FLOAT)
{
	this->init(vertices.size_bytes(), vertices.data());
}

vertex_buffer::vertex_buffer(utki::span<const r4::vector3<float>> vertices) :
		morda::vertex_buffer(vertices.size()),
		numComponents(3),
		type(GL_FLOAT)
{
	this->init(vertices.size_bytes(), vertices.data());
}

vertex_buffer::vertex_buffer(utki::span<const r4::vector2<float>> vertices) :
		morda::vertex_buffer(vertices.size()),
		numComponents(2),
		type(GL_FLOAT)
{
	this->init(vertices.size_bytes(), vertices.data());
}

vertex_buffer::vertex_buffer(utki::span<const float> vertices) :
		morda::vertex_buffer(vertices.size()),
		numComponents(1),
		type(GL_FLOAT)
{
	this->init(vertices.size_bytes(), vertices.data());
}
