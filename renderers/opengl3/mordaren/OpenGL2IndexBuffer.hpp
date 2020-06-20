#pragma once

#include <utki/Buf.hpp>

#include <morda/render/IndexBuffer.hpp>

#include "opengl_buffer.hpp"

class OpenGL2IndexBuffer : public morda::IndexBuffer, public opengl_buffer{
public:
	const GLenum elementType;
	const GLsizei elementsCount;
	
	OpenGL2IndexBuffer(utki::span<const std::uint16_t> indices);
	
	OpenGL2IndexBuffer(const OpenGL2IndexBuffer&) = delete;
	OpenGL2IndexBuffer& operator=(const OpenGL2IndexBuffer&) = delete;
	
	
private:

};
