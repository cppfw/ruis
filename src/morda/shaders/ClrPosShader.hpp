/* 
 * File:   ClrPosShader.hpp
 * Author: ivan
 *
 * Created on May 10, 2016, 2:03 PM
 */

#pragma once

#include <utki/Buf.hpp>

#include "PosShader.hpp"

namespace morda{

class ClrPosShader  :
		protected PosShader,
		virtual public Shader
{
	Render::InputID colorAttr;
	
public:
	ClrPosShader();
	
	ClrPosShader(const ClrPosShader&) = delete;
	ClrPosShader& operator=(const ClrPosShader&) = delete;
	
	void render(const utki::Buf<kolme::Vec2f> p, const utki::Buf<std::uint32_t> c, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal(p, c, mode);
	}
	
	void render(const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec4f> c, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal(p, c, mode);
	}
	
	void render(const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec4f> c, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec3f>(p, c, mode);
	}
	
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec4f> c, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec2f>(i, p, c, mode);
	}
	
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec4f> c, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec3f>(i, p, c, mode);
	}
	
private:
	template <class V, class C> void renderInternal(const utki::Buf<V> p, const utki::Buf<C> c, Render::Mode_e mode){
		if(p.size() != c.size()){
			TRACE(<< "ClrPosShader::renderInternal(): passed in array sizes do not match: p.size() = " << p.size() << " c.size() = " << c.size() << std::endl)
			throw morda::Exc("ClrPosShader::renderInternal(): passed in array sizes do not match");
		}
		this->setVertexAttribArray(this->positionAttr, p);
		this->setVertexAttribArray(this->colorAttr, c);
		this->renderArrays(mode, p.size());
	}
	
	template <class V, class C> void renderInternal(const utki::Buf<std::uint16_t> i, const utki::Buf<V> p, const utki::Buf<C> c, Render::Mode_e mode){
		if(p.size() != c.size()){
			TRACE(<< "ClrPosShader::renderInternal(): passed in array sizes do not match: p.size() = " << p.size() << " c.size() = " << c.size() << std::endl)
			throw morda::Exc("ClrPosShader::renderInternal(): passed in array sizes do not match");
		}
		this->setVertexAttribArray(this->positionAttr, p);
		this->setVertexAttribArray(this->colorAttr, c);
		this->renderElements(mode, i);
	}
};

}
