#pragma once

#include "../../../src/morda/render/Renderer.hpp"


#include "OpenGL2Factory.hpp"


class OpenGL2Renderer : public morda::Renderer{
	GLuint defaultFramebuffer;
public:
	OpenGL2Renderer(std::unique_ptr<OpenGL2Factory> factory);
	
	OpenGL2Renderer(const OpenGL2Renderer& orig);

	
	void setFramebufferInternal(morda::FrameBuffer_n* fb) override;

};

