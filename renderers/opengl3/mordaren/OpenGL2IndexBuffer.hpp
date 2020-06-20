#pragma once

#include <utki/Buf.hpp>

#include <morda/render/IndexBuffer.hpp>

#include "opengl_buffer.hpp"

class index_buffer : public morda::IndexBuffer, public opengl_buffer{
public:
	const GLenum elementType;
	const GLsizei elementsCount;
	
	index_buffer(utki::span<const std::uint16_t> indices);
	
	index_buffer(const index_buffer&) = delete;
	index_buffer& operator=(const index_buffer&) = delete;
	
	
private:

};
