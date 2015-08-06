#include "FrameBuffer.hpp"

#include "../Exc.hpp"
#include "Texture2D.hpp"



using namespace morda;


namespace{

ting::Void* curFBO = nullptr;

}


FrameBuffer::FrameBuffer(){
	this->fbo = Render::createFrameBuffer();
}


void FrameBuffer::attachColor(Texture2D&& tex){
	if(!this->isBound()){
		throw morda::Exc("FrameBuffer::attachColor(): framebuffer is not bound");
	}
	
	if(this->color.tex){
		throw morda::Exc("FrameBuffer::attachColor(): color attachment is already attached");
	}
	
	Render::attachColorTexture2DToFrameBuffer(tex.tex.get());
	
	this->color = std::move(tex);
}

Texture2D FrameBuffer::detachColor(){
	if(!this->isBound()){
		throw morda::Exc("FrameBuffer::detachColor(): framebuffer is not bound");
	}
	
	Render::attachColorTexture2DToFrameBuffer(nullptr);
	
	return std::move(this->color);
}

void FrameBuffer::bind(){
	if(this->isBound()){
		throw morda::Exc("FrameBuffer::bind(): framebuffer is already bound");
	}
	
	Render::bindFrameBuffer(this->fbo.get());
	this->prevFBO = curFBO;
	curFBO = this->fbo.get();
	this->isBound_var = true;
}

void FrameBuffer::unbind(){
	if(!this->isBound()){
		throw morda::Exc("FrameBuffer::unbind(): framebuffer is not bound");
	}
	
	Render::bindFrameBuffer(this->prevFBO);
	curFBO = this->prevFBO;
	this->prevFBO = nullptr;
	this->isBound_var = false;
}
