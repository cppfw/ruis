/* The MIT License:

Copyright (c) 2012-2014 Ivan Gagis

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


#include "PosShader.hpp"
#include "../Exc.hpp"


namespace morda{



class PosTexShader : public PosShader{
	GLuint texCoordAttr;

	GLuint texNumberUniform;
	
protected:
	void SetTexCoordPointer(const morda::Vec2f *p){
		glEnableVertexAttribArray(this->texCoordAttr);
		ASSERT(glGetError() == GL_NO_ERROR)
		ASSERT(p)
		glVertexAttribPointer(this->texCoordAttr, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLfloat*>(p));
		ASSERT(glGetError() == GL_NO_ERROR)
	}
public:
	static const std::array<Vec2f, 4> quadFanTexCoords;
	
	PosTexShader();
	
	PosTexShader(const PosTexShader&) = delete;
	PosTexShader& operator=(const PosTexShader&) = delete;
	
	//TODO: remove this
	void SetTextureNumber(unsigned i){
		glUniform1i(this->texNumberUniform, i);
		ASSERT(glGetError() == GL_NO_ERROR)
	}
	
	void Render(ting::Buffer<const Vec2f> p, ting::Buffer<const Vec2f> t, EMode mode = EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec2f>(p, t, mode);
	}
	
	void Render(ting::Buffer<const Vec3f> p, ting::Buffer<const Vec2f> t, EMode mode = EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec3f>(p, t, mode);
	}
	
private:
	template <class V> void RenderInternal(ting::Buffer<const V> p, ting::Buffer<const Vec2f> t, EMode mode){
		if(p.size() != t.size()){
			TRACE(<< "PosTexShader::RenderInternal(): passed in array sizes do not match: p.size() = " << p.size() << " t.size() = " << t.size() << std::endl)
			throw morda::Exc("PosTexShader::RenderInternal(): passed in array sizes do not match");
		}
		this->SetPositionPointer(&*p.begin());
		this->SetTexCoordPointer(&*t.begin());
		this->DrawArrays(GLint(mode), p.size());
	}
};



}//~namespace
