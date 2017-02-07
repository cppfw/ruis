#pragma once

#include <morda/render/VertexArray.hpp>

#include <GL/glew.h>

namespace mordaren{

class OpenGL2VertexArray : public morda::VertexArray{
public:	
	OpenGL2VertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, Mode_e mode);
	
	OpenGL2VertexArray(const OpenGL2VertexArray&) = delete;
	OpenGL2VertexArray& operator=(const OpenGL2VertexArray&) = delete;

	
private:

};

}
