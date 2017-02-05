#pragma once

#include <utki/Buf.hpp>

#include <morda/render/IndexBuffer.hpp>

#include "OpenGL2Buffer.hpp"

namespace mordaren{

class OpenGL2IndexBuffer : public morda::IndexBuffer, public OpenGL2Buffer{
public:
	const GLenum elementType;
	const GLsizei elementsCount;
	
	OpenGL2IndexBuffer(const utki::Buf<std::uint16_t> indices);
	
	OpenGL2IndexBuffer(const OpenGL2IndexBuffer&) = delete;
	OpenGL2IndexBuffer& operator=(const OpenGL2IndexBuffer&) = delete;
	
	
private:

};

}
