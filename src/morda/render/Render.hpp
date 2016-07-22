#pragma once

#include <memory>

#include <utki/Buf.hpp>
#include <utki/Void.hpp>

#include <kolme/Matrix4.hpp>
#include <kolme/Rectangle.hpp>


namespace morda{


class Shader;


/**
 * @brief Common interface to unify different rendering APIs.
 * Currently, only OpenGL 2.0+ and OpenGL ES 2.0+ are supported.
 */
class Render{
	friend class Shader;
	friend class Texture2D;
	friend class FrameBuffer;
	
	friend class App;
	
	std::unique_ptr<utki::Void> pimpl;
	
	Render();
	~Render()noexcept;
	
public:
	
	/**
	 * @brief Vertex data rendering mode.
	 * Enumeration defining how to interpret vertex data when rendering.
	 */
	enum class Mode_e{
		//NOTE: do not change order!!!
		
		TRIANGLES,
		TRIANGLE_FAN,
		LINE_LOOP,
		TRIANGLE_STRIP
	};
	
	/**
	 * @brief Encapsulation of shader input.
	 * This class can hold information about shader input, i.e. either an attribute or uniform.
	 */
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
	static void renderArrays(Mode_e mode, size_t numElements);
	
	static void renderElements(Mode_e mode, const utki::Buf<std::uint16_t>& i);
	
	//returns pointer to shader object
	static std::unique_ptr<utki::Void> compileShader(const char* vertexShaderCode, const char* fragmentShaderCode);
	
	static void bindShader(utki::Void& p);
	
	static InputID getAttribute(utki::Void& p, const char* n);
	
	static InputID getUniform(utki::Void& p, const char* n);
	
	static void setUniformMatrix4f(InputID id, const kolme::Matr4f& m);
	
	static void setUniform1i(InputID id, int i);
	
	static void setUniform2f(InputID id, kolme::Vec2f v);
	
	static void setUniform4f(InputID id, float x, float y, float z, float a);
	
	static void setUniform4f(InputID id, const utki::Buf<kolme::Vec4f> v);
	
	static void setVertexAttribArray(InputID id, const std::uint32_t* a);
	
	static void setVertexAttribArray(InputID id, const kolme::Vec4f* a);
	
	static void setVertexAttribArray(InputID id, const kolme::Vec3f* a);
	
	static void setVertexAttribArray(InputID id, const kolme::Vec2f* a);
	//=== ~~~
	
	
public:
	/**
	 * @brief Set rendering viewport.
	 * @param r - rectangle defining viewport in native window coordinates. X axis goes to the right, Y axis goes upwards.
	 */
	static void setViewport(kolme::Recti r);
	
	/**
	 * @brief Get current viewport.
	 * @return Rectangle defining current viewport.
	 */
	static kolme::Recti getViewport();
	
	/**
	 * @brief Clear color buffer.
	 * Sets all pixels in current color buffer to a given value.
	 * @param color - RGBA color value to set all pixels of color buffer to.
	 */
	static void clearColor(kolme::Vec4f color = kolme::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	
	/**
	 * @brief Clear depth buffer.
	 * Sets all pixels of depth buffer to a given value.
	 * @param d - value to set all pixels of depth buffer to.
	 */
	static void clearDepth(float d = 0.0f);
	
	/**
	 * @brief Clear stencil buffer.
	 * Sets all pixels of stencil buffer to a given value.
	 * @param v - value to set all pixels of stencil buffer to.
	 */
	static void clearStencil(std::uint8_t v = 0);
	
	/**
	 * @brief Check if scissor test is currently enabled.
	 * See "OpenGL scissor test" for details.
	 * @return true if scissor test is enabled.
	 * @return false otherwise.
	 */
	static bool isScissorEnabled();
	
	/**
	 * @brief Get current scissor test rectangle.
	 * The scissor test rectangle in viewport coordinates, X axis goes to the right, Y axis goes upwards.
	 * @return Current rectangle for scissor test.
	 */
	static kolme::Recti getScissorRect();
	
	/**
	 * @brief Enable or disable the scissor test.
	 * @param enabled - whether to enable (true) or disable (false) the scissor test.
	 */
	static void setScissorEnabled(bool enabled);
	
	/**
	 * @brief Set rectangle for scissor test.
	 * @param r - rectangle to set for scissor testing, in viewport coordinates, X axis goes to the right, Y axis goes upwards.
	 */
	static void setScissorRect(kolme::Recti r);
	
	/**
	 * @brief Get maximum texture size supported by rendering API.
	 * @return maximum texture size.
	 */
	static unsigned getMaxTextureSize();
	
	/**
	 * @brief Texture filtering method.
	 * Enumeration defining the texture filtering method to use when rendering.
	 */
	enum class TexFilter_e{
		//NOTE: do not change the order!!!
		
		NEAREST,
		LINEAR
	};
	
	/**
	 * @brief Enable/disable face culling.
	 * When face culling is enabled only one side of the face is rendered.
	 * @param enable - whether to enable or disable face culling.
	 */
	static void setCullEnabled(bool enable);
	
	//=== functions to be used by Texture class
private:
	static std::unique_ptr<utki::Void> create2DTexture(kolme::Vec2ui dim, unsigned numChannels, const utki::Buf<std::uint8_t> data, TexFilter_e minFilter, TexFilter_e magFilter);
	
	static void bindTexture(utki::Void& tex, unsigned unitNum);
	
	static bool isTextureBound(utki::Void& tex, unsigned unitNum);
	//=== ~~~
	
public:
	/**
	 * @brief Unbind texture from given texture unit.
	 * @param unitNum - number of the texture unit to unbind texture from.
	 */
	static void unbindTexture(unsigned unitNum);
	
	/**
	 * @brief Copy piece of color buffer to texture.
	 * Copy a rectangular piece of color buffer to a 0th texture.
	 * @param dst - position on the destination texture to put the copied rectangle.
	 * @param src - rectangle on the current color buffer. X axis goes right, Y axis goes upwards.
	 */
	static void copyColorBufferToTexture(kolme::Vec2i dst, kolme::Recti src);

	/**
	 * @brief Swap front and back buffers.
	 */
	static void swapFrameBuffers();
	
	//=== functions to be used by FrameBuffer class
private:
	static std::unique_ptr<utki::Void> createFrameBuffer();
	
	static void bindFrameBuffer(utki::Void* fbo);
	
	static void attachColorTexture2DToFrameBuffer(utki::Void* tex);
	
	/**
	 * @brief Is currently bound framebuffer complete.
	 * @return true if currently bound frasmebuffer is complete.
	 * @return false otherwise.
	 */
	static bool isFrameBufferComplete();
	//=== ~~~
	
	
public:
	/**
	 * @brief Enable/disale alpha blending.
	 * @param enable - whether to enable or disable alpha blending.
	 */
	static void setBlendEnabled(bool enable);
	
	/**
	 * @brief Blending factor type.
	 * Enumeration defines possible blending factor types.
	 */
	enum class BlendFactor_e{
		//NOTE: do not change order
		
		ZERO,
		ONE,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,
		CONSTANT_COLOR,
		ONE_MINUS_CONSTANT_COLOR,
		CONSTANT_ALPHA,
		ONE_MINUS_CONSTANT_ALPHA,
		SRC_ALPHA_SATURATE
	};
	
	/**
	 * @brief Set blending function to use when rendering with alpha blending enabled.
	 * @param srcClr - blending factor for source color component.
	 * @param dstClr - blending factor for destination color component.
	 * @param srcAlpha - blending factor for source alpha component.
	 * @param dstAlpha - blending factor for destination alpha component.
	 */
	static void setBlendFunc(BlendFactor_e srcClr, BlendFactor_e dstClr, BlendFactor_e srcAlpha, BlendFactor_e dstAlpha);

	/**
	 * @brief Checks if currently bound famebuffer is complete.
	 * Framebuffer is complete if it can be used for rendering. See OpenGL description of framebuffer completeness for details.
	 * @return true if currently bound framebuffer is complete.
	 * @return false otherwise.
	 */
	static bool isBoundFrameBufferComplete();
};



}
