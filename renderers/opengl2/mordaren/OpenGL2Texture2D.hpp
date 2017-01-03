#pragma once

#include <morda/render/Texture2D.hpp>

#ifdef M_RENDER_OPENGLES2
#	include <GLES2/gl2.h>
#else
#	include <GL/glew.h>
#endif


struct OpenGL2Texture2D : public morda::Texture2D{
	GLuint tex;
	
	OpenGL2Texture2D(kolme::Vec2f dim);
	
	~OpenGL2Texture2D()noexcept;
	
	void bind(unsigned unitNum)const;
};
