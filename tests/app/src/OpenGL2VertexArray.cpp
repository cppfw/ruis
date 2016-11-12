#include "OpenGL2VertexArray.hpp"


OpenGL2VertexArray::OpenGL2VertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers) :
		morda::VertexArray(std::move(buffers))
{
	glGenVertexArrays(1, &this->arr);
	//TODO: check for error
}

OpenGL2VertexArray::~OpenGL2VertexArray()noexcept{
	glDeleteVertexArrays(1, &this->arr);
	//TODO: check for error
}
