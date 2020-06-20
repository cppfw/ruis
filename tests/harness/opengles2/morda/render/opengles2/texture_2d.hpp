#pragma once

#include <utki/config.hpp>

#include <morda/render/texture_2d.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

namespace morda{ namespace render_opengles2{

struct texture_2d : public morda::texture_2d{
	GLuint tex;
	
	texture_2d(r4::vec2f dims);
	
	~texture_2d()noexcept;
	
	void bind(unsigned unitNum)const;
};

}}
