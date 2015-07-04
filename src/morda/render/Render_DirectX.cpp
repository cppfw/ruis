

#include <memory>

#include <ting/Exc.hpp>
#include <ting/Void.hpp>
#include <ting/PoolStored.hpp>
#include <ting/Buffer.hpp>

#include "../Exc.hpp"

using namespace morda;

namespace{



//GLenum modeMap[] = {
//	GL_TRIANGLES,			//TRIANGLES
//	GL_TRIANGLE_FAN,		//TRIANGLE_FAN
//	GL_LINE_LOOP			//LINE_LOOP
//};



}



void Render::renderArrays(EMode mode, unsigned numElements) {
//TODO:
}



void Render::renderElements(EMode mode, const ting::Buffer<const std::uint16_t>& i) {
	//TODO:
}

void Render::bindShader(ting::Void& p) {
	//TODO:
}

std::unique_ptr<ting::Void> Render::compileShader(const char* vertexShaderCode, const char* fragmentShaderCode) {
	//TODO:
	return nullptr;
}


Render::InputID Render::getAttribute(ting::Void& p, const char* n) {
	//TODO:
	return InputID(0);
}

Render::InputID Render::getUniform(ting::Void& p, const char* n) {
	//TODO:
	return InputID(0);
}

void Render::setUniformMatrix4f(InputID id, const Matr4f& m) {
	//TODO:
}

void Render::setUniform1i(InputID id, int i) {
	//TODO:
}

void Render::setUniform2f(InputID id, Vec2f v) {
	//TODO:
}

void Render::setUniform4f(InputID id, float x, float y, float z, float a) {
	//TODO:
}

void Render::setUniform4f(InputID id, ting::Buffer<const Vec4f> v) {
	//TODO:
}

void Render::setVertexAttribArray(InputID id, const Vec3f* a) {
	//TODO:
}

void Render::setVertexAttribArray(InputID id, const Vec2f* a) {
	//TODO:
}

void Render::setViewport(Rect2i r){
	//TODO:
}

void Render::setClearColor(Vec4f c){
	//TODO:
}

Render::Render(){
	//TODO:
}

Render::~Render()noexcept{
	
}

void Render::clear(EBuffer b) {
	/*
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
	}*/
	//TODO:
}

bool Render::isScissorEnabled() {
	//TODO:
	return false;
}

Rect2i Render::getScissorRect() {
	//TODO:
	return Rect2i(0);
}

void Render::setScissorEnabled(bool enabled) {
	//TODO:
}

void Render::setScissorRect(Rect2i r) {
	//TODO:
}


namespace{

/*
GLint texFilterMap[] = {
	GL_NEAREST,
	GL_LINEAR
};
*/

/*
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
};*/

}//~namespace

std::unique_ptr<ting::Void> Render::create2DTexture(Vec2ui dim, unsigned numChannels, ting::Buffer<const std::uint8_t> data, ETexFilter minFilter, ETexFilter magFilter){
	/*
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
	*/
	//TODO:
	return nullptr;
}

void Render::bindTexture(ting::Void& tex, unsigned unitNum){
	//TODO:
}

void Render::copyColorBufferToTexture(Vec2i dst, Rect2i src){
	//TODO:
}

unsigned Render::getMaxTextureSize(){
	//TODO:
	return 1024;
}
