#pragma once

#include "Render.hpp"

#include "Texture2D.hpp"

namespace morda{

/**
 * @brief A class for framebuffer encapsulation.
 */
class FrameBuffer {
	bool isBound_v = false;
	
	utki::Void* prevFBO = nullptr;
	std::unique_ptr<utki::Void> fbo;
	
	Texture2D color;
public:
	/**
	 * @brief Creates empty framebuffer object.
	 */
	FrameBuffer();

	/**
	 * @brief Bind framebuffer object to current rendering context.
	 * Unbinds the previously bound frambeuffer and pushes it to stack.
	 * When this framebuffer is unbound it will restore the previous frambuffer binding.
	 */
	void bind();
	
	/**
	 * @brief Unbind this framebuffer object.
	 * Unbinds this framebuffer from rendering context, pops previous frambuffer from stack and
	 * binds it to rendering context.
	 */
	void unbind();
	
	/**
	 * @brief Check if this framebuffer is currently bound to rendering context.
	 * @return true if this framebuffer is bound to rendering context.
	 * @return false otherwise.
	 */
	bool isBound(){
		return this->isBound_v;
	}
	
	/**
	 * @brief Attach color buffer to this framebuffer.
	 * Attaches the color buffer to this framebuffer. Note, that this framebuffer should
	 * be currently bound.
	 * @param color - texture to use as a color buffer for this framebuffer.
	 * @throw morda::Exc if the framebuffer is not currently bound.
	 * @throw morda::Exc if this framebuffer already has color buffer attached.
	 */
	void attachColor(Texture2D&& color);
	
	/**
	 * @brief Detach color buffer from this framebuffer.
	 * The framebuffer should be currently bound.
	 * @return texture which was attached as color buffer.
	 * @throw morda::Exc if this framebuffer is not curretly bound.
	 */
	Texture2D detachColor();
private:

};

}
