/* The MIT License:

Copyright (c) 2015 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: https://github.com/igagis/morda

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Render.hpp"

#include "Texture2D.hpp"

namespace morda{


class FrameBuffer {
	bool isBound_var = false;
	
	ting::Void* prevFBO = nullptr;
	std::unique_ptr<ting::Void> fbo;
	
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
