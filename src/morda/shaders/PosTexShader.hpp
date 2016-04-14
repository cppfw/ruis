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
	static const std::array<kolme::Vec2f, 4> quadFanTexCoords;
	static const std::array<kolme::Vec2f, 4> quadFanTexCoordsUpsideDown;
	
	PosTexShader();
	
	PosTexShader(const PosTexShader&) = delete;
	PosTexShader& operator=(const PosTexShader&) = delete;
	
	void render(const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec2f>(p, t, mode);
	}
	
	void render(const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec3f>(p, t, mode);
	}
	
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec2f>(i, p, t, mode);
	}
	
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec2f> t, Render::EMode mode = Render::EMode::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec3f>(i, p, t, mode);
	}
	
private:
	template <class V> void renderInternal(const utki::Buf<V> p, const utki::Buf<kolme::Vec2f> t, Render::EMode mode){
		if(p.size() != t.size()){
			TRACE(<< "PosTexShader::RenderInternal(): passed in array sizes do not match: p.size() = " << p.size() << " t.size() = " << t.size() << std::endl)
			throw morda::Exc("PosTexShader::RenderInternal(): passed in array sizes do not match");
		}
		this->setVertexAttribArray(this->positionAttr, p);
		this->setVertexAttribArray(this->texCoordAttr, t);
		this->renderArrays(mode, p.size());
	}
	
	template <class V> void renderInternal(const utki::Buf<std::uint16_t> i, const utki::Buf<V> p, const utki::Buf<kolme::Vec2f> t, Render::EMode mode){
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
