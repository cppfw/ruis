#pragma once

#include <utki/Buf.hpp>

#include <kolme/Vector2.hpp>

#include <morda/render/VertexBuffer.hpp>

#include "opengl_buffer.hpp"

class OpenGL2VertexBuffer : public morda::VertexBuffer, public opengl_buffer{
public:
	const GLint numComponents;
	const GLenum type;
	
	OpenGL2VertexBuffer(utki::span<const kolme::Vec4f> vertices);
	
	OpenGL2VertexBuffer(utki::span<const kolme::Vec3f> vertices);
	
	OpenGL2VertexBuffer(utki::span<const kolme::Vec2f> vertices);
	
	OpenGL2VertexBuffer(const OpenGL2VertexBuffer&) = delete;
	OpenGL2VertexBuffer& operator=(const OpenGL2VertexBuffer&) = delete;

private:
	void init(GLsizeiptr size, const GLvoid* data);
};


