#pragma once

#include <utki/span.hpp>

#include <morda/render/index_buffer.hpp>

#include "OpenGL2Buffer.hpp"

namespace mordaren{

class OpenGL2IndexBuffer :
		public morda::index_buffer,
		public OpenGL2Buffer
{
public:
	const GLenum elementType;
	const GLsizei elementsCount;
	
	OpenGL2IndexBuffer(utki::span<const std::uint16_t> indices);
	
	OpenGL2IndexBuffer(const OpenGL2IndexBuffer&) = delete;
	OpenGL2IndexBuffer& operator=(const OpenGL2IndexBuffer&) = delete;
	
	
private:

};

}
