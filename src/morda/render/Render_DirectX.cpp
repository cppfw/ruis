#include <d3d11.h>

#include <memory>

#include <utki/Exc.hpp>
#include <utki/Void.hpp>
#include <utki/PoolStored.hpp>
#include <utki/Buf.hpp>

#include "../Exc.hpp"

#include "../App.hpp"

using namespace morda;


namespace {

struct Direct3DContext : public utki::Void {
	IDXGISwapChain *swapchain;
	ID3D11Device *dev;
	ID3D11DeviceContext *ctx;

	ID3D11RenderTargetView *renderTarget;

	Direct3DContext(HWND hwnd) {
		HRESULT result;

		// create a struct to hold information about the swap chain
		DXGI_SWAP_CHAIN_DESC scd;

		// clear out the struct for use
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		// fill the swap chain description struct
		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = hwnd;                                // the window to be used
		scd.SampleDesc.Count = 4;                               // how many multisamples
		scd.Windowed = TRUE;                                    // windowed/full-screen mode

																// create a device, device context and swap chain using the information in the scd struct
		result = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			NULL,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&scd,
			&this->swapchain,
			&this->dev,
			NULL,
			&this->ctx);
		if (FAILED(result)) {
			throw morda::Exc("Direct3D init: creating device failed");
		}

		// get the address of the back buffer
		ID3D11Texture2D *pBackBuffer;
		result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(result)) {
			this->swapchain->Release();
			this->dev->Release();
			this->ctx->Release();
			throw morda::Exc("Direct3D init: creating device failed");
		}

		// use the back buffer address to create the render target
		result = dev->CreateRenderTargetView(pBackBuffer, NULL, &this->renderTarget);
		pBackBuffer->Release();
		if (FAILED(result)) {
			this->swapchain->Release();
			this->dev->Release();
			this->ctx->Release();
			throw morda::Exc("Direct3D init: creating render target failed");
		}

		// set the render target as the back buffer
		this->ctx->OMSetRenderTargets(1, &this->renderTarget, NULL);
	}

	~Direct3DContext()noexcept {
		this->renderTarget->Release();
		this->swapchain->Release();
		this->dev->Release();
		this->ctx->Release();
	}
} *d3d;


//GLenum modeMap[] = {
//	GL_TRIANGLES,			//TRIANGLES
//	GL_TRIANGLE_FAN,		//TRIANGLE_FAN
//	GL_LINE_LOOP			//LINE_LOOP
//};


}//~namespace



void Render::renderArrays(EMode mode, unsigned numElements) {
//TODO:
}



void Render::renderElements(EMode mode, const const utki::Buf<std::uint16_t>& i) {
	//TODO:
}

void Render::bindShader(utki::Void& p) {
	//TODO:
}

std::unique_ptr<utki::Void> Render::compileShader(const char* vertexShaderCode, const char* fragmentShaderCode) {
	//TODO:
	return nullptr;
}


Render::InputID Render::getAttribute(utki::Void& p, const char* n) {
	//TODO:
	return InputID(0);
}

Render::InputID Render::getUniform(utki::Void& p, const char* n) {
	//TODO:
	return InputID(0);
}

void Render::setUniformMatrix4f(InputID id, const kolme::Matr4f& m) {
	//TODO:
}

void Render::setUniform1i(InputID id, int i) {
	//TODO:
}

void Render::setUniform2f(InputID id, kolme::Vec2f v) {
	//TODO:
}

void Render::setUniform4f(InputID id, float x, float y, float z, float a) {
	//TODO:
}

void Render::setUniform4f(InputID id, const utki::Buf<Vec4f> v) {
	//TODO:
}

void Render::setVertexAttribArray(InputID id, const Vec3f* a) {
	//TODO:
}

void Render::setVertexAttribArray(InputID id, const kolme::Vec2f* a) {
	//TODO:
}

void Render::setViewport(Rect2i r){
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = FLOAT(r.p.x);
	viewport.TopLeftY = FLOAT(r.p.y);
	viewport.Width = FLOAT(r.d.x);
	viewport.Height = FLOAT(r.d.y);

	d3d->ctx->RSSetViewports(1, &viewport);
}



Render::Render(){
	d3d = new Direct3DContext(morda::App::inst().window.hwnd);
	this->pimpl.reset(d3d);
}

Render::~Render()noexcept{
	
}

void Render::clearColor(Vec4f color) {
	FLOAT clr[4] = {
			color.x,
			color.y,
			color.z,
			color.w
		};
	d3d->ctx->ClearRenderTargetView(d3d->renderTarget, clr);
}

void Render::clearDepth(float d) {
	d3d->ctx->ClearDepthStencilView(NULL, D3D11_CLEAR_DEPTH, FLOAT(d), 0);//TODO: NULL?
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
struct GLTexture2D : public utki::Void, public ting::PoolStored<GLTexture2D, 32>{
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

std::unique_ptr<utki::Void> Render::create2DTexture(Vec2ui dim, unsigned numChannels, const utki::Buf<std::uint8_t> data, ETexFilter minFilter, ETexFilter magFilter){
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

void Render::bindTexture(utki::Void& tex, unsigned unitNum){
	//TODO:
}

void Render::copyColorBufferToTexture(Vec2i dst, Rect2i src){
	//TODO:
}

unsigned Render::getMaxTextureSize(){
	//TODO:
	return 1024;
}


void Render::swapFrameBuffers() {
	d3d->swapchain->Present(0, 0);
}