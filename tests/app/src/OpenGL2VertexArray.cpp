#include "OpenGL2VertexArray.hpp"


OpenGL2VertexArray::OpenGL2VertexArray(){
	glGenVertexArrays(1, &this->buffer);
	//TODO: check for error
}

OpenGL2VertexArray::~OpenGL2VertexArray()noexcept{
	glDeleteVertexArrays(1, &this->buffer);
	//TODO: check for error
}
