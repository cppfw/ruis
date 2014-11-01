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
	GLint positionAttr;
	
public:	
	
	static const std::array<Vec2f, 4> quadFan;
	
	static const std::array<Vec2f, 4> quad01Fan;
		
	
	PosShader(const PosShader&) = delete;
	PosShader& operator=(const PosShader&) = delete;
	
	
protected:
	PosShader(){
		this->positionAttr = this->GetAttribute("pos");
	}
	
	void SetPositionPointer(const morda::Vec3f *p){
		ASSERT(this->IsBound())
		glEnableVertexAttribArray(this->positionAttr);
		ASSERT(glGetError() == GL_NO_ERROR)
		ASSERT(p)
		glVertexAttribPointer(this->positionAttr, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*>(p));
		ASSERT(glGetError() == GL_NO_ERROR)
	}

	void SetPositionPointer(const morda::Vec2f *p){
		ASSERT(this->IsBound())
		glEnableVertexAttribArray(this->positionAttr);
		ASSERT(glGetError() == GL_NO_ERROR)
		ASSERT(p)
		glVertexAttribPointer(this->positionAttr, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*>(p));
		ASSERT(glGetError() == GL_NO_ERROR)
	}
	
	
public:
	void Render(ting::Buffer<const Vec2f> p, EMode mode = EMode::TRIANGLE_FAN){
		this->Render<Vec2f>(p, mode);
	}
	
	void Render(ting::Buffer<const Vec3f> p, EMode mode = EMode::TRIANGLE_FAN){
		this->Render<Vec3f>(p, mode);
	}
	
	void Render(ting::Buffer<const std::uint16_t> i, ting::Buffer<const Vec2f> p, EMode mode = EMode::TRIANGLE_FAN){
		this->Render<Vec2f>(i, p , mode);
	}
	
	void Render(ting::Buffer<const std::uint16_t> i, ting::Buffer<const Vec3f> p, EMode mode = EMode::TRIANGLE_FAN){
		this->Render<Vec3f>(i, p , mode);
	}
	
private:
	template <class V> void Render(ting::Buffer<const V> p, EMode mode){
		this->SetPositionPointer(&*p.begin());
		this->DrawArrays(GLint(mode), p.size());
	}
	
	template <class V> void Render(ting::Buffer<const std::uint16_t> i, ting::Buffer<const V> p, EMode mode){
		this->SetPositionPointer(&*p.begin());
		this->DrawElements(GLint(mode), i);
	}
};

}
