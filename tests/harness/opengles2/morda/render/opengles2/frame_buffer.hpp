#pragma once

#include <utki/config.hpp>

#include <morda/render/frame_buffer.hpp>
#include <morda/render/texture_2d.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

namespace morda{ namespace render_opengles2{

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
