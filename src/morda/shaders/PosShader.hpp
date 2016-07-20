#pragma once

#include <utki/Buf.hpp>

#include "../render/Shader.hpp"


namespace morda{

	
/**
 * @brief Position shader.
 * Basic shader which has vertex position attribute.
 * Vertex position attribute should be 'vec4' and named 'pos' in GLSL.
 */
class PosShader : virtual public Shader{
public:
	
	static const std::array<kolme::Vec2f, 4> quadFan;
	
	static const std::array<kolme::Vec2f, 4> quad01Fan;
	
	
	PosShader(const PosShader&) = delete;
	PosShader& operator=(const PosShader&) = delete;
	
	
protected:
	PosShader() :
			Shader(nullptr, nullptr),
			positionAttr(this->getAttribute("pos"))
	{}
	
	/**
	 * @brief Position attribute ID.
	 * This variable is initialized at the moment of shader object construction.
	 * After the shader program is compiled it will search for attribute named 'pos'
	 * and assign its ID to this variable.
	 */
	const Render::InputID positionAttr;
	
public:
	/**
	 * @brief Render array of 2 dimensional vertices.
	 * @param p - array of vertices to render.
	 * @param mode - vertex rendering mode.
	 */
	void render(const utki::Buf<kolme::Vec2f> p, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->render<kolme::Vec2f>(p, mode);
	}
	
	/**
	 * @brief Render array of 3 dimensional vertices.
	 * @param p - array of vertices to render.
	 * @param mode - vertex rendering mode.
	 */
	void render(const utki::Buf<kolme::Vec3f> p, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->render<kolme::Vec3f>(p, mode);
	}
	
	/**
	 * @brief Render 2 dimensianl vertices using indices.
	 * @param i - array of vertex indices to render.
	 * @param p - array of vertices to use for rendering.
	 * @param mode - vertex rendering mode.
	 */
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec2f> p, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->render<kolme::Vec2f>(i, p, mode);
	}
	
	/**
	 * @brief Render 3 dimensianl vertices using indices.
	 * @param i - array of vertex indices to render.
	 * @param p - array of vertices to use for rendering.
	 * @param mode - vertex rendering mode.
	 */
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
