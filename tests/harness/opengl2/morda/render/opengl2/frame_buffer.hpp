#pragma once

#include <morda/render/frame_buffer.hpp>
#include <morda/render/texture_2d.hpp>

#include <GL/glew.h>

namespace morda{ namespace render_opengl2{

class frame_buffer : public morda::frame_buffer{
public:
	GLuint fbo;
	
	frame_buffer(std::shared_ptr<morda::texture_2d> color);
	
	frame_buffer(const frame_buffer&) = delete;
	frame_buffer& operator=(const frame_buffer&) = delete;
	
	~frame_buffer()noexcept;
private:

};

}}
