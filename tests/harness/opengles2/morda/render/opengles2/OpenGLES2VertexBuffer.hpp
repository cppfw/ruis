#pragma once

#include <utki/span.hpp>

#include <r4/vector2.hpp>

#include <morda/render/vertex_buffer.hpp>

#include "opengl_buffer.hpp"

namespace morda{ namespace render_opengles2{

class OpenGLES2VertexBuffer : public morda::vertex_buffer, public opengl_buffer{
public:
	const GLint numComponents;
	const GLenum type;
	
	OpenGLES2VertexBuffer(utki::span<const r4::vec4f> vertices);
	
	OpenGLES2VertexBuffer(utki::span<const r4::vec3f> vertices);
	
	OpenGLES2VertexBuffer(utki::span<const r4::vec2f> vertices);
	
	OpenGLES2VertexBuffer(utki::span<const float> vertices);
	
	OpenGLES2VertexBuffer(const OpenGLES2VertexBuffer&) = delete;
	OpenGLES2VertexBuffer& operator=(const OpenGLES2VertexBuffer&) = delete;

private:
	void init(GLsizeiptr size, const GLvoid* data);
};

}}
