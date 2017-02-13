#pragma once

#include <utki/Buf.hpp>

#include <kolme/Vector2.hpp>

#include <morda/render/VertexBuffer.hpp>

#include "OpenGLES2Buffer.hpp"

namespace mordaren{

class OpenGLES2VertexBuffer : public morda::VertexBuffer, public OpenGLES2Buffer{
public:
	const GLint numComponents;
	const GLenum type;
	
	OpenGLES2VertexBuffer(const utki::Buf<kolme::Vec4f> vertices);
	
	OpenGLES2VertexBuffer(const utki::Buf<kolme::Vec3f> vertices);
	
	OpenGLES2VertexBuffer(const utki::Buf<kolme::Vec2f> vertices);
	
	OpenGLES2VertexBuffer(const OpenGLES2VertexBuffer&) = delete;
	OpenGLES2VertexBuffer& operator=(const OpenGLES2VertexBuffer&) = delete;

private:
	void init(GLsizeiptr size, const GLvoid* data);
};


}
