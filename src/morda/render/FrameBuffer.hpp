/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Render.hpp"

#include "Texture2D.hpp"

namespace morda{


class FrameBuffer {
	bool isBound_var = false;
	
	utki::Void* prevFBO = nullptr;
	std::unique_ptr<utki::Void> fbo;
	
	Texture2D color;
public:
	FrameBuffer();

	void bind();
	
	void unbind();
	
	bool isBound(){
		return this->isBound_var;
	}
	
	void attachColor(Texture2D&& color);
	
	Texture2D detachColor();
	
	bool isComplete();
private:

};

}
