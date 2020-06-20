#pragma once

#include <utki/Buf.hpp>

#include <kolme/Vector2.hpp>

#include <morda/render/VertexBuffer.hpp>

#include "opengl_buffer.hpp"

class vertex_buffer : public morda::VertexBuffer, public opengl_buffer{
public:
	const GLint numComponents;
	const GLenum type;
	
	vertex_buffer(utki::span<const kolme::Vec4f> vertices);
	
	vertex_buffer(utki::span<const kolme::Vec3f> vertices);
	
	vertex_buffer(utki::span<const kolme::Vec2f> vertices);
	
	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;

private:
	void init(GLsizeiptr size, const GLvoid* data);
};


