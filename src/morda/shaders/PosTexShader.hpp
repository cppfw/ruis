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



class PosTexShader :
		protected PosShader,
		virtual public Shader
{
	Render::InputID texCoordAttr;

	Render::InputID texNumberUniform;
	
protected:

public:
	static const std::array<Vec2f, 4> quadFanTexCoords;
	
	PosTexShader();
	
	PosTexShader(const PosTexShader&) = delete;
	PosTexShader& operator=(const PosTexShader&) = delete;
	
	void SetTextureNumber(unsigned i){
		this->setUniform1i(this->texNumberUniform, i);
	}
	
	void render(ting::Buffer<const Vec2f> p, ting::Buffer<const Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec2f>(p, t, mode);
	}
	
	void render(ting::Buffer<const Vec3f> p, ting::Buffer<const Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec3f>(p, t, mode);
	}
	
	void render(ting::Buffer<const std::uint16_t> i, ting::Buffer<const Vec2f> p, ting::Buffer<const Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec2f>(i, p, t, mode);
	}
	
	void render(ting::Buffer<const std::uint16_t> i, ting::Buffer<const Vec3f> p, ting::Buffer<const Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec3f>(i, p, t, mode);
	}
	
private:
	template <class V> void RenderInternal(ting::Buffer<const V> p, ting::Buffer<const Vec2f> t, Render::EMode mode){
		if(p.size() != t.size()){
			TRACE(<< "PosTexShader::RenderInternal(): passed in array sizes do not match: p.size() = " << p.size() << " t.size() = " << t.size() << std::endl)
			throw morda::Exc("PosTexShader::RenderInternal(): passed in array sizes do not match");
		}
		this->setVertexAttribArray(this->positionAttr, p);
		this->setVertexAttribArray(this->texCoordAttr, t);
		this->renderArrays(mode, p.size());
	}
	
	template <class V> void RenderInternal(ting::Buffer<const std::uint16_t> i, ting::Buffer<const V> p, ting::Buffer<const Vec2f> t, Render::EMode mode){
		if(p.size() != t.size()){
			TRACE(<< "PosTexShader::RenderInternal(): passed in array sizes do not match: p.size() = " << p.size() << " t.size() = " << t.size() << std::endl)
			throw morda::Exc("PosTexShader::RenderInternal(): passed in array sizes do not match");
		}
		this->setVertexAttribArray(this->positionAttr, p);
		this->setVertexAttribArray(this->texCoordAttr, t);
		this->renderElements(mode, i);
	}
};



}//~namespace
