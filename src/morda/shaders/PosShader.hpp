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
