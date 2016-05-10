/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <utki/Buf.hpp>

#include "../render/Shader.hpp"


namespace morda{

	
/**
 * @brief Position shader.
 * Vertex position attribute should be 'vec4' and named 'pos'.
 */
class PosShader : virtual public Shader{
public:
	
	static const std::array<kolme::Vec2f, 4> quadFan;
	
	static const std::array<kolme::Vec2f, 4> quad01Fan;
	
	
	PosShader(const PosShader&) = delete;
	PosShader& operator=(const PosShader&) = delete;
	
	
protected:
	PosShader() :
			positionAttr(this->getAttribute("pos"))
	{}
	
	const Render::InputID positionAttr;
	
public:
	void render(const utki::Buf<kolme::Vec2f> p, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->render<kolme::Vec2f>(p, mode);
	}
	
	void render(const utki::Buf<kolme::Vec3f> p, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->render<kolme::Vec3f>(p, mode);
	}
	
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec2f> p, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->render<kolme::Vec2f>(i, p, mode);
	}
	
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec3f> p, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->render<kolme::Vec3f>(i, p, mode);
	}
	
private:
	template <class V> void render(const utki::Buf<V> p, Render::Mode_e mode){
		this->setVertexAttribArray(this->positionAttr, p);
		this->renderArrays(mode, p.size());
	}
	
	template <class V> void render(const utki::Buf<std::uint16_t> i, const utki::Buf<V> p, Render::Mode_e mode){
		this->setVertexAttribArray(this->positionAttr, p);
		this->renderElements(mode, i);
	}
};

}
