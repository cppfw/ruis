#pragma once

#include <utki/config.hpp>

#include <morda/render/vertex_array.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif


namespace mordaren{

class OpenGLES2VertexArray : public morda::vertex_array{
public:	
	OpenGLES2VertexArray(std::vector<std::shared_ptr<morda::vertex_buffer>>&& buffers, std::shared_ptr<morda::index_buffer> indices, mode rendering_mode);
	
	OpenGLES2VertexArray(const OpenGLES2VertexArray&) = delete;
	OpenGLES2VertexArray& operator=(const OpenGLES2VertexArray&) = delete;

	
private:

};

}
