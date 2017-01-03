#pragma once

#include <morda/render/VertexArray.hpp>

#ifdef M_RENDER_OPENGLES2
#	include <GLES2/gl2.h>
#else
#	include <GL/glew.h>
#endif


class OpenGL2VertexArray : public morda::VertexArray{
public:	
	OpenGL2VertexArray(std::vector<std::shared_ptr<morda::VertexBuffer>>&& buffers, std::shared_ptr<morda::IndexBuffer> indices, Mode_e mode);
	
	OpenGL2VertexArray(const OpenGL2VertexArray&) = delete;
	OpenGL2VertexArray& operator=(const OpenGL2VertexArray&) = delete;

	
private:

};

