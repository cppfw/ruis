/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <memory>

#include <utki/Buf.hpp>
#include <utki/Void.hpp>

#include "../util/Matrix4.hpp"
#include "../util/Rectangle2.hpp"


namespace morda{


class Shader;


class Render{
	friend class Shader;
	friend class Texture2D;
	friend class FrameBuffer;
	
	friend class App;
	
	std::unique_ptr<ting::Void> pimpl;
	
	Render();
	~Render()noexcept;
	
public:
	
	enum class EMode{
		//NOTE: do not change order!!!
		
		TRIANGLES,
		TRIANGLE_FAN,
		LINE_LOOP
	};
	
	class InputID{
		friend class morda::Render;
		std::int64_t id;
		void* data;
	public:
		InputID(std::int64_t id, void* data = nullptr) :
			id(id),
			data(data)
		{}
	};
	
	//=== functions to be used by Shader class internally
private:
	static void renderArrays(EMode mode, unsigned numElements);
	
	static void renderElements(EMode mode, const ting::Buffer<const std::uint16_t>& i);
	
	//returns pointer to shader object
	static std::unique_ptr<ting::Void> compileShader(const char* vertexShaderCode, const char* fragmentShaderCode);
	
	static void bindShader(ting::Void& p);
	
	static InputID getAttribute(ting::Void& p, const char* n);
	
	static InputID getUniform(ting::Void& p, const char* n);
	
	static void setUniformMatrix4f(InputID id, const Matr4f& m);
	
	static void setUniform1i(InputID id, int i);
	
	static void setUniform2f(InputID id, Vec2f v);
	
	static void setUniform4f(InputID id, float x, float y, float z, float a);
	
	static void setUniform4f(InputID id, ting::Buffer<const Vec4f> v);
	
	static void setVertexAttribArray(InputID id, const Vec3f* a);
	
	static void setVertexAttribArray(InputID id, const Vec2f* a);
	//=== ~~~
	
	
public:
	
	static void setViewport(Rect2i r);
	
	static Rect2i getViewport();
	
	static void clearColor(Vec4f c = Vec4f(0.0f, 0, 0.0f, 1.0f));
	
	static void clearDepth(float d = 0.0f);
	
	static bool isScissorEnabled();
	
	static Rect2i getScissorRect();
	
	static void setScissorEnabled(bool enabled);
	
	static void setScissorRect(Rect2i r);
	
	static unsigned getMaxTextureSize();
	
	enum class ETexFilter{
		//NOTE: do not change the order!!!
		
		NEAREST,
		LINEAR
	};
	
	static void setCullEnabled(bool enable);
	
	//=== functions to be used by Texture class
private:
	static std::unique_ptr<ting::Void> create2DTexture(Vec2ui dim, unsigned numChannels, ting::Buffer<const std::uint8_t> data, ETexFilter minFilter, ETexFilter magFilter);
	
	static void bindTexture(ting::Void& tex, unsigned unitNum);
	//=== ~~~
	
public:
	
	static void unbindTexture(unsigned unitNum);
	
	static void copyColorBufferToTexture(Vec2i dst, Rect2i src);

	static void swapFrameBuffers();
	
	//=== functions to be used by FrameBuffer class
private:
	static std::unique_ptr<ting::Void> createFrameBuffer();
	
	static void bindFrameBuffer(ting::Void* fbo);
	
	static void attachColorTexture2DToFrameBuffer(ting::Void* tex);
	//=== ~~~
	
	
public:
	static void setBlendEnabled(bool enable);
	
	enum class EBlendFactor{
		//NOTE: do not change order
		
		ONE,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		ONE_MINUS_DST_ALPHA
	};
	
	static void setBlendFunc(EBlendFactor srcClr, EBlendFactor dstClr, EBlendFactor srcAlpha, EBlendFactor dstAlpha);

	static bool isBoundFrameBufferComplete();

};



}
