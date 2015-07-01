/* The MIT License:

Copyright (c) 2014 Ivan Gagis <igagis@gmail.com>

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

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <ting/Buffer.hpp>

#include "Shader.hpp"


namespace morda{

	
/**
 * @brief Position shader.
 * Vertex position attribute should be 'vec4' and named 'pos'.
 */
class PosShader : virtual public Shader{
public:
	
	static const std::array<Vec2f, 4> quadFan;
	
	static const std::array<Vec2f, 4> quad01Fan;
	
	
	PosShader(const PosShader&) = delete;
	PosShader& operator=(const PosShader&) = delete;
	
	
protected:
	PosShader() :
			positionAttr(this->getAttribute("pos"))
	{}
	
	const Render::InputID positionAttr;
	
public:
	void render(ting::Buffer<const Vec2f> p, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->render<Vec2f>(p, mode);
	}
	
	void render(ting::Buffer<const Vec3f> p, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->render<Vec3f>(p, mode);
	}
	
	void render(ting::Buffer<const std::uint16_t> i, ting::Buffer<const Vec2f> p, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->render<Vec2f>(i, p, mode);
	}
	
	void render(ting::Buffer<const std::uint16_t> i, ting::Buffer<const Vec3f> p, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->render<Vec3f>(i, p, mode);
	}
	
private:
	template <class V> void render(ting::Buffer<const V> p, Render::EMode mode){
		this->setVertexAttribArray(this->positionAttr, p);
		this->renderArrays(mode, p.size());
	}
	
	template <class V> void render(ting::Buffer<const std::uint16_t> i, ting::Buffer<const V> p, Render::EMode mode){
		this->setVertexAttribArray(this->positionAttr, p);
		this->renderElements(mode, i);
	}
};

}
