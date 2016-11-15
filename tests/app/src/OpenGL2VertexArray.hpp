#pragma once

#include "../../../src/morda/render/VertexArray.hpp"

#include <GL/glew.h>


class OpenGL2VertexArray : public morda::VertexArray{
public:
	const GLuint arr;
	
	OpenGL2VertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, Mode_e mode);
	
	OpenGL2VertexArray(const OpenGL2VertexArray&) = delete;
	OpenGL2VertexArray& operator=(const OpenGL2VertexArray&) = delete;
	
	~OpenGL2VertexArray()noexcept;
	
private:

};

