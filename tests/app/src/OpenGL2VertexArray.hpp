#pragma once

#include "../../../src/morda/render/VertexArray.hpp"

#include <GL/glew.h>


class OpenGL2VertexArray : public morda::VertexArray{
	GLuint buffer;
public:
	OpenGL2VertexArray();
	
	OpenGL2VertexArray(const OpenGL2VertexArray&) = delete;
	OpenGL2VertexArray& operator=(const OpenGL2VertexArray&) = delete;
	
	~OpenGL2VertexArray()noexcept;
	
private:

};

