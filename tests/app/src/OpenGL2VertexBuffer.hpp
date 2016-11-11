#pragma once

#include <utki/Buf.hpp>

#include <kolme/Vector2.hpp>

#include "../../../src/morda/render/VertexBuffer.hpp"

#include "OpenGL2Buffer.hpp"

class OpenGL2VertexBuffer : public morda::VertexBuffer, public OpenGL2Buffer{
	
public:
	OpenGL2VertexBuffer(const utki::Buf<kolme::Vec3f> vertices);
	
	OpenGL2VertexBuffer(const OpenGL2VertexBuffer&) = delete;
	OpenGL2VertexBuffer& operator=(const OpenGL2VertexBuffer&) = delete;
	
	
	
private:

};


