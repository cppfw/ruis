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
	
protected:

public:
	static const std::array<Vec2f, 4> quadFanTexCoords;
	
	PosTexShader();
	
	PosTexShader(const PosTexShader&) = delete;
	PosTexShader& operator=(const PosTexShader&) = delete;
	
	void render(const utki::Buf<Vec2f> p, const utki::Buf<Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec2f>(p, t, mode);
	}
	
	void render(const utki::Buf<Vec3f> p, const utki::Buf<Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec3f>(p, t, mode);
	}
	
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<Vec2f> p, const utki::Buf<Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec2f>(i, p, t, mode);
	}
	
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<Vec3f> p, const utki::Buf<Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->RenderInternal<Vec3f>(i, p, t, mode);
	}
	
private:
	template <class V> void RenderInternal(const utki::Buf<V> p, const utki::Buf<Vec2f> t, Render::EMode mode){
		if(p.size() != t.size()){
			TRACE(<< "PosTexShader::RenderInternal(): passed in array sizes do not match: p.size() = " << p.size() << " t.size() = " << t.size() << std::endl)
			throw morda::Exc("PosTexShader::RenderInternal(): passed in array sizes do not match");
		}
		this->setVertexAttribArray(this->positionAttr, p);
		this->setVertexAttribArray(this->texCoordAttr, t);
		this->renderArrays(mode, p.size());
	}
	
	template <class V> void RenderInternal(const utki::Buf<std::uint16_t> i, const utki::Buf<V> p, const utki::Buf<Vec2f> t, Render::EMode mode){
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
