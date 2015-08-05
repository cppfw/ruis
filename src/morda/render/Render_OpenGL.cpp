#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES
#	include <GLES2/gl2.h>
#else
#	include <GL/glew.h>
#endif

#if M_OS == M_OS_WINDOWS
#	include <ting/windows.hpp>
#endif

#include <memory>
#include <sstream>

#include <ting/Exc.hpp>
#include <ting/Void.hpp>
#include <ting/PoolStored.hpp>
#include <ting/Buffer.hpp>

#include "../Exc.hpp"
#include "../App.hpp"

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



const char* shaderDefs =
#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES
R"qwertyuiop(
		#define MAT4F highp mat4
		#define VEC2F highp vec2
		#define VEC4F highp vec4
	)qwertyuiop"
#else
R"qwertyuiop(
		#define MAT4F mat4
		#define VEC2F vec2
		#define VEC4F vec4
	)qwertyuiop"
#endif

	R"qwertyuiop(
		#define UNIFORM(type, name) uniform type name;

		#define UNIFORM_BEGIN
		#define UNIFORM_END

		#define ATTRIB(type, name) attribute type name;

		#define ATTRIB_BEGIN
		#define ATTRIB_END

		#define VARYING(type, name) varying type name;

		#define VARYING_BEGIN
		#define VARYING_END

		#define VERTEX_MAIN_BEGIN void main(void){

		#define VERTEX_MAIN_END }

		#define FRAG_MAIN_BEGIN void main(void){
		
		#define FRAG_MAIN_END }

		#define OUT(varying_name) varying_name

		#define VARYING_POS

		#define OUT_POS gl_Position
		#define OUT_FRAGCOLOR gl_FragColor

		#define TEXTURE2D(name) sampler2D name;
	)qwertyuiop";


struct ShaderWrapper{
	GLuint s;
	ShaderWrapper(const char* code, GLenum type){
		this->s = glCreateShader(type);
	
		if(this->s == 0){
			throw ting::Exc("glCreateShader() failed");
		}

		std::stringstream ss;
		ss << shaderDefs << code;
		auto cstr = ss.str();
		const char* c = cstr.c_str();

		glShaderSource(this->s, 1, &c, 0);
		glCompileShader(this->s);
		if(this->CheckForCompileErrors(this->s)){
			TRACE(<< "Error while compiling:\n" << c << std::endl)
			glDeleteShader(this->s);
			throw ting::Exc("Error compiling shader");
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




namespace {

#if M_OS == M_OS_WINDOWS

struct OpenGLContext : public ting::Void{
	HGLRC hrc;
public:
	OpenGLContext(HDC hdc) {
		//	TRACE_AND_LOG(<< "App::GLContextWrapper::GLContextWrapper(): enter" << std::endl)

		this->hrc = wglCreateContext(hdc);
		if (!this->hrc) {
			throw morda::Exc("Failed to create OpenGL rendering context");
		}

		//	TRACE_AND_LOG(<< "App::GLContextWrapper::GLContextWrapper(): GL rendering context created" << std::endl)

		if (!wglMakeCurrent(hdc, this->hrc)) {
			this->Destroy();
			throw morda::Exc("Failed to activate OpenGL rendering context");
		}

		//	TRACE_AND_LOG(<< "App::GLContextWrapper::GLContextWrapper(): GL rendering context created" << std::endl)
	}

	~OpenGLContext()noexcept {
		this->Destroy();
	}

	void Destroy() {
		if (!wglMakeCurrent(NULL, NULL)) {
			ASSERT_INFO(false, "Deactivating OpenGL rendering context failed")
		}

		if (!wglDeleteContext(this->hrc)) {
			ASSERT_INFO(false, "Releasing OpenGL rendering context failed")
		}
	}
};

#else

#endif

}//~namespace

Render::Render() :
		pimpl(
#if M_OS == M_OS_WINDOWS
				new OpenGLContext(morda::App::Inst().deviceContext.hdc)
#endif
			)
{
#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGL
	if(glewInit() != GLEW_OK){
		throw morda::Exc("GLEW initialization failed");
	}
#endif
	AssertOpenGLNoError();
	TRACE_AND_LOG(<< "OpenGL version: " << glGetString(GL_VERSION) << std::endl)
}

Render::~Render()noexcept {

}

void Render::clearColor(Vec4f c) {
	glClearColor(c.x, c.y, c.z, c.w);
	AssertOpenGLNoError();
	glClear(GL_COLOR_BUFFER_BIT);
	AssertOpenGLNoError();
}

void Render::clearDepth(float d) {
	glClear(GL_DEPTH_BUFFER_BIT);
	AssertOpenGLNoError();
}


bool Render::isScissorEnabled() {
	return glIsEnabled(GL_SCISSOR_TEST) ? true : false; //?true:false is to avoid warning under MSVC
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


namespace{

GLint texFilterMap[] = {
	GL_NEAREST,
	GL_LINEAR
};

struct GLTexture2D : public ting::Void, public ting::PoolStored<GLTexture2D, 32>{
	GLuint tex;
	
	GLTexture2D(){
		glGenTextures(1, &this->tex);
		AssertOpenGLNoError();
		ASSERT(this->tex != 0)
	}
	
	virtual ~GLTexture2D()noexcept{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &this->tex);
	}
	
	void bind(unsigned unitNum){
		glActiveTexture(GL_TEXTURE0 + unitNum);
		AssertOpenGLNoError();
		glBindTexture(GL_TEXTURE_2D, this->tex);
		AssertOpenGLNoError();
	}
};

}//~namespace

std::unique_ptr<ting::Void> Render::create2DTexture(Vec2ui dim, unsigned numChannels, ting::Buffer<const std::uint8_t> data, ETexFilter minFilter, ETexFilter magFilter){
	ASSERT(data.size() == 0 || data.size() >= dim.x * dim.y * numChannels)
	
	GLint minFilterGL = texFilterMap[unsigned(minFilter)];
	GLint magFilterGL = texFilterMap[unsigned(magFilter)];
	
	std::unique_ptr<GLTexture2D> ret(new GLTexture2D());
	
	ret->bind(0);
	
	GLint internalFormat;
	switch(numChannels){
		default:
			ASSERT(false)
		case 1:
			internalFormat = GL_LUMINANCE;
			break;
		case 2:
			internalFormat = GL_LUMINANCE_ALPHA;
			break;
		case 3:
			internalFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA;
			break;
	}

	//we will be passing pixels to OpenGL which are 1-byte aligned.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	AssertOpenGLNoError();

	glTexImage2D(
			GL_TEXTURE_2D,
			0,//0th level, no mipmaps
			internalFormat, //internal format
			dim.x,
			dim.y,
			0,//border, should be 0!
			internalFormat, //format of the texel data
			GL_UNSIGNED_BYTE,
			&*data.begin()
		);
	AssertOpenGLNoError();

	//NOTE: on OpenGL ES 2 it is necessary to set the filter parameters
	//      for every texture!!! Otherwise it may not work!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterGL);
	AssertOpenGLNoError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterGL);
	AssertOpenGLNoError();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	return std::move(ret);
}

void Render::bindTexture(ting::Void& tex, unsigned unitNum){
	static_cast<GLTexture2D&>(tex).bind(unitNum);
}

void Render::copyColorBufferToTexture(Vec2i dst, Rect2i src){
	glCopyTexSubImage2D(
		GL_TEXTURE_2D,
		0, //level
		dst.x, //xoffset
		dst.y, //yoffset
		src.p.x,
		src.p.y,
		src.d.x,
		src.d.y
	);
	AssertOpenGLNoError();
}

unsigned Render::getMaxTextureSize(){
	GLint val;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &val);
	ASSERT(val > 0)
	return unsigned(val);
}

void Render::swapFrameBuffers() {
#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES
	eglSwapBuffers(morda::App::Inst().eglDisplay.d, morda::App::Inst().eglSurface.s);
#else
	static_assert(M_MORDA_RENDER == M_MORDA_RENDER_OPENGL, "Unexpected render API");
#	if M_OS == M_OS_WINDOWS
	SwapBuffers(morda::App::Inst().deviceContext.hdc);
#	elif M_OS == M_OS_LINUX
	glXSwapBuffers(morda::App::Inst().xDisplay.d, morda::App::Inst().xWindow.w);
#	elif M_OS == M_OS_MACOSX
	morda::App::Inst().macosx_SwapFrameBuffers();
#	else
#		error "unknown OS"
#	endif
#endif
}

void Render::setCullEnabled(bool enable) {
	if(enable){
		glEnable(GL_CULL_FACE);
	}else{
		glDisable(GL_CULL_FACE);
	}
}
