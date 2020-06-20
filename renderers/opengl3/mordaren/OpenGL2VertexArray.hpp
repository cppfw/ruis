#pragma once

#include <morda/render/VertexArray.hpp>

#include <GL/glew.h>


class vertex_array : public morda::VertexArray{
public:
	const GLuint arr;
	
	vertex_array(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, Mode_e mode);
	
	vertex_array(const vertex_array&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;
	
	~vertex_array()noexcept;
	
private:

};

