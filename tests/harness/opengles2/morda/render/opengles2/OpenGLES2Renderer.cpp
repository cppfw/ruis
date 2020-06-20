#include <utki/config.hpp>

#include "OpenGLES2Renderer.hpp"
#include "OpenGLES2FrameBuffer.hpp"
#include "OpenGLES2_util.hpp"

#if M_OS_NAME == M_OS_NAME_IOS
#	include <OpenGlES/ES2/glext.h>
#else
#	include <GLES2/gl2.h>
#endif

using namespace morda;

namespace{
unsigned getMaxTextureSize(){
	GLint val;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &val);
	ASSERT(val > 0)
	return unsigned(val);
}
}

OpenGLES2Renderer::OpenGLES2Renderer(std::unique_ptr<OpenGLES2Factory> factory) :
		morda::renderer(
				std::move(factory),
				[](){
					renderer::params p;
					p.max_texture_size = getMaxTextureSize();
					return p;
				}()
			)
{}

void OpenGLES2Renderer::set_framebuffer_internal(morda::frame_buffer* fb){
	if(!this->defaultFramebufferInitialized){
		// On some platforms the default framebuffer is not 0, so because of this
		// check if default framebuffer value is saved or not every time some
		// framebuffer is going to be bound and save the value if needed.
		GLint oldFb;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFb);
		TRACE(<< "oldFb = " << oldFb << std::endl)
		this->defaultFramebuffer = decltype(this->defaultFramebuffer)(oldFb);
		this->defaultFramebufferInitialized = true;
	}
	
	if(!fb){
		glBindFramebuffer(GL_FRAMEBUFFER, GLuint(this->defaultFramebuffer));
		assertOpenGLNoError();
		return;
	}
	
	ASSERT(dynamic_cast<OpenGLES2FrameBuffer*>(fb))
	auto& ogl2fb = static_cast<OpenGLES2FrameBuffer&>(*fb);
	
	glBindFramebuffer(GL_FRAMEBUFFER, ogl2fb.fbo);
	assertOpenGLNoError();
}

void OpenGLES2Renderer::clear_framebuffer(){
	glClearColor(0, 0, 0, 1);
	assertOpenGLNoError();
	glClear(GL_COLOR_BUFFER_BIT);
	assertOpenGLNoError();
	
	glClearDepthf(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	assertOpenGLNoError();
	
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	assertOpenGLNoError();
}

bool OpenGLES2Renderer::is_scissor_enabled()const{
	return glIsEnabled(GL_SCISSOR_TEST) ? true : false; // "? true : false" is to avoid warning under MSVC
}

void OpenGLES2Renderer::set_scissor_enabled(bool enabled){
	if(enabled){
		glEnable(GL_SCISSOR_TEST);
	}else{
		glDisable(GL_SCISSOR_TEST);
	}
}

r4::recti OpenGLES2Renderer::get_scissor()const{
	GLint osb[4];
	glGetIntegerv(GL_SCISSOR_BOX, osb);
	return r4::recti(osb[0], osb[1], osb[2], osb[3]);
}

void OpenGLES2Renderer::set_scissor(r4::recti r){
	glScissor(r.p.x, r.p.y, r.d.x, r.d.y);
	assertOpenGLNoError();
}

r4::recti OpenGLES2Renderer::get_viewport()const{
	GLint vp[4];

	glGetIntegerv(GL_VIEWPORT, vp);
	
	return r4::recti(vp[0], vp[1], vp[2], vp[3]);
}

void OpenGLES2Renderer::set_viewport(r4::recti r){
	glViewport(r.p.x, r.p.y, r.d.x, r.d.y);
	assertOpenGLNoError();
}

void OpenGLES2Renderer::set_blend_enabled(bool enable){
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

void OpenGLES2Renderer::set_blend_func(blend_factor src_color, blend_factor dst_color, blend_factor src_alpha, blend_factor dst_alpha){
	glBlendFuncSeparate(
			blendFunc[unsigned(src_color)],
			blendFunc[unsigned(dst_color)],
			blendFunc[unsigned(src_alpha)],
			blendFunc[unsigned(dst_alpha)]
		);
}
