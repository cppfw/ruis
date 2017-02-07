#include "OpenGL2_util.hpp"
#include "OpenGL2Renderer.hpp"
#include "OpenGL2FrameBuffer.hpp"

#include <utki/config.hpp>

using namespace mordaren;

namespace{
unsigned getMaxTextureSize(){
	GLint val;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &val);
	ASSERT(val > 0)
	return unsigned(val);
}
}

OpenGL2Renderer::OpenGL2Renderer(std::unique_ptr<OpenGL2Factory> factory) :
		morda::Renderer(std::move(factory), getMaxTextureSize())
{
	//On some platforms the default framebuffer is not 0, so because of this
	//check if default framebuffer value is saved or not everytime some
	//framebuffer is going to be bound and save the value if needed.
	GLint oldFb;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
	TRACE(<< "oldFb = " << oldFb << std::endl)
	this->defaultFramebuffer = GLuint(oldFb);
}

void OpenGL2Renderer::setFramebufferInternal(morda::FrameBuffer* fb) {
	if(!fb){
		glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebuffer);
		assertOpenGLNoError();
		return;
	}
	
	ASSERT(dynamic_cast<OpenGL2FrameBuffer*>(fb))
	auto& ogl2fb = static_cast<OpenGL2FrameBuffer&>(*fb);
	
	glBindFramebuffer(GL_FRAMEBUFFER, ogl2fb.fbo);
	assertOpenGLNoError();
}

void OpenGL2Renderer::clearFramebuffer() {
	glClearColor(0, 0, 0, 1);
	assertOpenGLNoError();
	glClear(GL_COLOR_BUFFER_BIT);
	assertOpenGLNoError();
	
#if M_OS_NAME == M_OS_NAME_IOS || defined(M_RENDER_OPENGLES2)
	glClearDepthf(0);
#else
	glClearDepth(0);
#endif
	glClear(GL_DEPTH_BUFFER_BIT);
	assertOpenGLNoError();
	
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	assertOpenGLNoError();
}

bool OpenGL2Renderer::isScissorEnabled() const {
	return glIsEnabled(GL_SCISSOR_TEST) ? true : false; //?true:false is to avoid warning under MSVC
}

void OpenGL2Renderer::setScissorEnabled(bool enabled) {
	if(enabled){
		glEnable(GL_SCISSOR_TEST);
	}else{
		glDisable(GL_SCISSOR_TEST);
	}
}

kolme::Recti OpenGL2Renderer::getScissorRect() const {
	GLint osb[4];
	glGetIntegerv(GL_SCISSOR_BOX, osb);
	return kolme::Recti(osb[0], osb[1], osb[2], osb[3]);
}

void OpenGL2Renderer::setScissorRect(kolme::Recti r) {
	glScissor(r.p.x, r.p.y, r.d.x, r.d.y);
	assertOpenGLNoError();
}

kolme::Recti OpenGL2Renderer::getViewport()const {
	GLint vp[4];

	glGetIntegerv(GL_VIEWPORT, vp);
	
	return kolme::Recti(vp[0], vp[1], vp[2], vp[3]);
}

void OpenGL2Renderer::setViewport(kolme::Recti r) {
	glViewport(r.p.x, r.p.y, r.d.x, r.d.y);
	assertOpenGLNoError();
}

void OpenGL2Renderer::setBlendEnabled(bool enable) {
	if(enable){
		glEnable(GL_BLEND);
	}else{
		glDisable(GL_BLEND);
	}
}

namespace{

GLenum blendFunc[] = {
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_CONSTANT_ALPHA,
	GL_ONE_MINUS_CONSTANT_ALPHA,
	GL_SRC_ALPHA_SATURATE
};

}

void OpenGL2Renderer::setBlendFunc(BlendFactor_e srcClr, BlendFactor_e dstClr, BlendFactor_e srcAlpha, BlendFactor_e dstAlpha) {
	glBlendFuncSeparate(
			blendFunc[unsigned(srcClr)],
			blendFunc[unsigned(dstClr)],
			blendFunc[unsigned(srcAlpha)],
			blendFunc[unsigned(dstAlpha)]
		);
}
