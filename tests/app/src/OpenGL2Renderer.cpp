#include "OpenGL2Renderer.hpp"

#include "OpenGL2FrameBuffer.hpp"

OpenGL2Renderer::OpenGL2Renderer(std::unique_ptr<OpenGL2Factory> factory) :
		morda::Renderer(std::move(factory))
{
	//On some platforms the default framebuffer is not 0, so because of this
	//check if default framebuffer value is saved or not everytime some
	//framebuffer is going to be bound and save the value if needed.
	GLint oldFb;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
	TRACE(<< "oldFb = " << oldFb << std::endl)
	this->defaultFramebuffer = GLuint(oldFb);
}

void OpenGL2Renderer::setFramebufferInternal(morda::FrameBuffer_n* fb) {
	if(!fb){
		glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebuffer);
		AssertOpenGLNoError();
		return;
	}
	
	ASSERT(dynamic_cast<OpenGL2FrameBuffer*>(fb))
	auto& ogl2fb = static_cast<OpenGL2FrameBuffer&>(*fb);
	
	glBindFramebuffer(GL_FRAMEBUFFER, ogl2fb.fbo);
	AssertOpenGLNoError();
}
