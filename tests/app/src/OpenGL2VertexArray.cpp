#include "OpenGL2VertexArray.hpp"

#include "OpenGL2_util.hpp"
#include "OpenGL2VertexBuffer.hpp"

OpenGL2VertexArray::OpenGL2VertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers) :
		morda::VertexArray(std::move(buffers))
{
	glGenVertexArrays(1, &this->arr);
	AssertOpenGLNoError();
	
	glBindVertexArray(this->arr);
	AssertOpenGLNoError();
	
	for(unsigned i = 0; i != this->buffers.size(); ++i){
		ASSERT(dynamic_cast<OpenGL2VertexBuffer*>(this->buffers[i].operator->()))
		auto& vbo = static_cast<OpenGL2VertexBuffer&>(*this->buffers[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo.buffer);

		glVertexAttribPointer(i, vbo.numComponents, vbo.type, GL_FALSE, 0, nullptr);
	}
	
	//TODO: remove this
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	AssertOpenGLNoError();
}

OpenGL2VertexArray::~OpenGL2VertexArray()noexcept{
	glDeleteVertexArrays(1, &this->arr);
	AssertOpenGLNoError();
}
