#pragma once

#include <morda/render/Texture2D.hpp>

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

namespace mordaren{

struct OpenGLES2Texture2D : public morda::Texture2D{
	GLuint tex;
	
	OpenGLES2Texture2D(kolme::Vec2f dim);
	
	~OpenGLES2Texture2D()noexcept;
	
	void bind(unsigned unitNum)const;
};


}
