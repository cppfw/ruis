#pragma once

#include <morda/render/VertexArray.hpp>

#include <GLES2/gl2.h>


namespace mordaren{

class OpenGLES2VertexArray : public morda::VertexArray{
public:	
	OpenGLES2VertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, Mode_e mode);
	
	OpenGLES2VertexArray(const OpenGLES2VertexArray&) = delete;
	OpenGLES2VertexArray& operator=(const OpenGLES2VertexArray&) = delete;

	
private:

};

}
