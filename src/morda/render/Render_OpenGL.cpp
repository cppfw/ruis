#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES
#	include <GLES2/gl2.h>
#else
#	include <GL/glew.h>
#endif

#include <ting/Exc.hpp>

#include "../Exc.hpp"

using namespace morda;

namespace{



inline static void AssertOpenGLNoError(){
#ifdef DEBUG
	GLenum error = glGetError();
	switch(error){
		case GL_NO_ERROR:
			return;
		case GL_INVALID_ENUM:
			ASSERT_INFO(false, "OpenGL error: GL_INVALID_ENUM")
			break;
		case GL_INVALID_VALUE:
			ASSERT_INFO(false, "OpenGL error: GL_INVALID_VALUE")
			break;
		case GL_INVALID_OPERATION:
			ASSERT_INFO(false, "OpenGL error: GL_INVALID_OPERATION")
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			ASSERT_INFO(false, "OpenGL error: GL_INVALID_FRAMEBUFFER_OPERATION")
			break;
		case GL_OUT_OF_MEMORY:
			ASSERT_INFO(false, "OpenGL error: GL_OUT_OF_MEMORY")
			break;
		default:
			ASSERT_INFO(false, "Unknown OpenGL error, code = " << int(error))
			break;
	}
#endif
}
	



GLenum modeMap[] = {
	GL_TRIANGLES,			//TRIANGLES
	GL_TRIANGLE_FAN,		//TRIANGLE_FAN
	GL_LINE_LOOP			//LINE_LOOP
};




struct ShaderWrapper{
	GLuint s;
	ShaderWrapper(const char* code, GLenum type){
		this->s = glCreateShader(type);
	
		if(this->s == 0){
			throw ting::Exc("glCreateShader() failed");
		}

		glShaderSource(this->s, 1, &code, 0);
		glCompileShader(this->s);
		if(this->CheckForCompileErrors(this->s)){
			TRACE(<< "Error while compiling:\n" << code << std::endl)
			glDeleteShader(this->s);
			throw ting::Exc("Error compiling vertex shader");
		}
	}
	~ShaderWrapper()NOEXCEPT{
		glDeleteShader(this->s);
	}

	//return true if not compiled
	static bool CheckForCompileErrors(GLuint shader){
		GLint value = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
		if(value == 0){ //if not compiled
			GLint logLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
			if(logLen > 1){//1 char is a terminating 0
				std::vector<char> log(logLen);
				GLint len;
				glGetShaderInfoLog(shader, log.size(), &len, &*log.begin());
				TRACE(<< "===Compile log===\n" << &*log.begin() << std::endl)
			}else{
				TRACE(<< "Shader compile log is empty" << std::endl)
			}
			return true;
		}
		return false;
	}
};


struct ProgramWrapper : public ting::Void{
	ShaderWrapper vertexShader;
	ShaderWrapper fragmentShader;
	GLuint p;
	ProgramWrapper(const char* vertexShaderCode, const char* fragmentShaderCode) :
			vertexShader(vertexShaderCode, GL_VERTEX_SHADER),
			fragmentShader(fragmentShaderCode, GL_FRAGMENT_SHADER)
	{
		this->p = glCreateProgram();
		glAttachShader(this->p, vertexShader.s);
		glAttachShader(this->p, fragmentShader.s);
		glLinkProgram(this->p);
		if(this->CheckForLinkErrors(this->p)){
			TRACE(<< "Error while linking shader program" << vertexShaderCode << std::endl << fragmentShaderCode << std::endl)
			glDeleteProgram(this->p);
			throw ting::Exc("Error linking shader program");
		}
	}

	virtual ~ProgramWrapper()NOEXCEPT{
		glDeleteProgram(this->p);
	}

	//return true if not linked
	static bool CheckForLinkErrors(GLuint program){
		GLint value = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &value);
		if(value == 0){ //if not linked
			GLint logLen = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
			if(logLen > 1){ //1 is for terminating 0 character.
				std::vector<char> log(logLen);
				GLint len;
				glGetProgramInfoLog(program, log.size(), &len, &*log.begin());
				TRACE(<< "===Link log===\n" << &*log.begin() << std::endl)
			}
			return true;
		}
		return false;
	}
};

}



void Render::renderArrays(EMode mode, unsigned numElements) {
	GLenum m = modeMap[unsigned(mode)];
	
	glDrawArrays(m, 0, numElements);
	AssertOpenGLNoError();
}



void Render::renderElements(EMode mode, const ting::Buffer<const std::uint16_t>& i) {
	GLenum m = modeMap[unsigned(mode)];
	
	glDrawElements(m, i.size(), GL_UNSIGNED_SHORT, &*i.begin());
	AssertOpenGLNoError();
}

void Render::bindShader(ting::Void& p) {
	glUseProgram(static_cast<ProgramWrapper&>(p).p);
	AssertOpenGLNoError();
}

std::unique_ptr<ting::Void> Render::compileShader(const char* vertexShaderCode, const char* fragmentShaderCode) {
	return std::unique_ptr<ProgramWrapper>(new ProgramWrapper(vertexShaderCode, fragmentShaderCode));
}


Render::InputID Render::getAttribute(ting::Void& p, const char* n) {
	GLint ret = glGetAttribLocation(static_cast<ProgramWrapper&>(p).p, n);
	if(ret < 0){
		std::stringstream ss;
		ss << "No attribute found in the shader program: " << n;
		throw ting::Exc(ss.str());
	}
	return InputID(ret);
}

Render::InputID Render::getUniform(ting::Void& p, const char* n) {
	GLint ret = glGetUniformLocation(static_cast<ProgramWrapper&>(p).p, n);
	if(ret < 0){
		throw ting::Exc("No uniform found in the shader program");
	}
	return InputID(ret);
}

void Render::setUniformMatrix4f(InputID id, const Matr4f& m) {
	glUniformMatrix4fv(GLint(id.id), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&m));
	AssertOpenGLNoError();
}

void Render::setUniform1i(InputID id, int i) {
	glUniform1i(GLint(id.id), i);
	AssertOpenGLNoError();
}

void Render::setUniform2f(InputID id, Vec2f v) {
	glUniform2f(GLint(id.id), v.x, v.y);
	AssertOpenGLNoError();
}

void Render::setUniform4f(InputID id, float x, float y, float z, float a) {
	glUniform4f(GLint(id.id), x, y, z, a);
	AssertOpenGLNoError();
}

void Render::setUniform4f(InputID id, ting::Buffer<const Vec4f> v) {
	static_assert(sizeof(v[0]) == sizeof(GLfloat) * 4, "size mismatch");
	glUniform4fv(GLint(id.id), v.size(), reinterpret_cast<const GLfloat*>(&*v.begin()));
	AssertOpenGLNoError();
}

void Render::setVertexAttribArray(InputID id, const Vec3f* a) {
	glEnableVertexAttribArray(GLint(id.id));
	AssertOpenGLNoError();
	ASSERT(a)
	glVertexAttribPointer(GLint(id.id), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*>(a));
	AssertOpenGLNoError();
}

void Render::setVertexAttribArray(InputID id, const Vec2f* a) {
	glEnableVertexAttribArray(GLint(id.id));
	AssertOpenGLNoError();
	ASSERT(a)
	glVertexAttribPointer(GLint(id.id), 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*>(a));
	AssertOpenGLNoError();
}

void Render::setViewport(Rect2i r){
	glViewport(r.p.x, r.p.y, r.d.x, r.d.y);
	AssertOpenGLNoError();
}

void Render::setClearColor(Vec4f c){
	glClearColor(c.x, c.y, c.z, c.w);
	AssertOpenGLNoError();
}

Render::Render(){
#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGL
	if(glewInit() != GLEW_OK){
		throw morda::Exc("GLEW initialization failed");
	}
#endif
	AssertOpenGLNoError();
	TRACE_AND_LOG(<< "OpenGL version: " << glGetString(GL_VERSION) << std::endl)
}

void Render::clear(EBuffer b) {
	GLbitfield bf = 0;
	switch(b){
		default:
		case EBuffer::COLOR:
			bf = GL_COLOR_BUFFER_BIT;
			break;
		case EBuffer::DEPTH:
			bf = GL_DEPTH_BUFFER_BIT;
			break;
		case EBuffer::ACCUM:
			bf = GL_ACCUM_BUFFER_BIT;
			break;
		case EBuffer::STENCIL:
			bf = GL_STENCIL_BUFFER_BIT;
			break;
	}
	glClear(bf);
	AssertOpenGLNoError();
}

bool Render::isScissorEnabled() {
	return glIsEnabled(GL_SCISSOR_TEST);
}

Rect2i Render::getScissorRect() {
	GLint osb[4];
	glGetIntegerv(GL_SCISSOR_BOX, osb);
	return Rect2i(osb[0], osb[1], osb[2], osb[3]);
}

void Render::setScissorEnabled(bool enabled) {
	if(enabled){
		glEnable(GL_SCISSOR_TEST);
	}else{
		glDisable(GL_SCISSOR_TEST);
	}
}

void Render::setScissorRect(Rect2i r) {
	glScissor(r.p.x, r.p.y, r.d.x, r.d.y);
	AssertOpenGLNoError();
}
