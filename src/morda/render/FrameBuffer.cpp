#include "FrameBuffer.hpp"

#include "../Exc.hpp"
#include "Texture2D.hpp"



using namespace morda;


namespace{

ting::Void* curFBO = nullptr;

}


FrameBuffer::FrameBuffer(){
	this->fbo = Render::createFrameBuffer();
	this->bind();
}


FrameBuffer::FrameBuffer(Texture2D&& tex) :
		FrameBuffer()
{
	this->color = std::move(tex);
	Render::attachColorTexture2DToFrameBuffer(this->color.tex.get());
}

void FrameBuffer::bind(){
	if(curFBO == this->fbo.get()){
		throw morda::Exc("FrameBuffer::bind(): framebuffer is already bound");
	}
	
	Render::bindFrameBuffer(this->fbo.get());
	this->prevFBO = curFBO;
	curFBO = this->fbo.get();
}

void FrameBuffer::unbind(){
	if(curFBO != this->fbo.get()){
		throw morda::Exc("FrameBuffer::unbind(): framebuffer is not bound");
	}
	
	Render::bindFrameBuffer(this->prevFBO);
	curFBO = this->prevFBO;
	this->prevFBO = nullptr;
}
