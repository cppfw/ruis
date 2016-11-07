#include "OpenGL2VertexBuffer.hpp"


OpenGL2VertexBuffer::OpenGL2VertexBuffer(const utki::Buf<kolme::Vec2f> vertices) :
		morda::VertexBuffer(vertices.size())
{
	glGenBuffers(1, &this->buffer);
	//TODO: check for error
}



OpenGL2VertexBuffer::~OpenGL2VertexBuffer()noexcept{
	glDeleteBuffers(1, &this->buffer);
	//TODO: check for error
}
