#pragma once


#include "PosShader.hpp"
#include "../Exc.hpp"


namespace morda{


/**
 * @brief Texturing shader with texture coordinates attribute and texture sampler uniform.
 * This shader implements a simple texturing shader.
 * This class can also be subclassed.
 * Texture coordinates attribute should be 'vec2f' and named 'texCoord0' in GLSL.
 * Texture sample uniform should be 'sampler2D' and named 'texture0' in GLSL.
 */
class PosTexShader :
		protected PosShader,
		virtual public Shader
{
	Render::InputID texCoordAttr;
	
protected:

public:
	/**
	 * @brief array of texture coordinates to use for rendering quad with whole texture.
	 * This is a convenience array of texture coordinates which can be used for
	 * rendering a quad in TRIANGLE_FAN mode using the whole texture.
	 */
	static const std::array<kolme::Vec2f, 4> quadFanTexCoords;
	
	/**
	 * @brief array of texture coordinates to use for rendering quad with whole texture.
	 * Same as 'quadFanTexCoords' but resulting in texture to appear up side down on the quad.
	 */
	static const std::array<kolme::Vec2f, 4> quadFanTexCoordsUpsideDown;
	
	PosTexShader();
	
	PosTexShader(const PosTexShader&) = delete;
	PosTexShader& operator=(const PosTexShader&) = delete;
	
	/**
	 * @brief Render array of 2 dimensional vertices.
	 * Sizes of 'p' and 't' should match.
	 * @param p - vertex positions.
	 * @param t - vertex texture coordinates.
	 * @param mode - vertex rendering mode.
	 * @throw morda::Exc if sizes of 'p' and 't' do not match.
	 */
	void render(const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec2f> t, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec2f>(p, t, mode);
	}
	
	/**
	 * @brief Render array of 3 dimensional vertices.
	 * Sizes of 'p' and 't' should match.
	 * @param p - vertex positions.
	 * @param t - vertex texture coordinates.
	 * @param mode - vertex rendering mode.
	 * @throw morda::Exc if sizes of 'p' and 't' do not match.
	 */
	void render(const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec2f> t, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec3f>(p, t, mode);
	}
	
	/**
	 * @brief Render 2 dimensional vertices using indices.
	 * Sizes of 'p' and 't' should match.
	 * @param i - vertex indices.
	 * @param p - vertex positions.
	 * @param t - vertex texture coordinates.
	 * @param mode - vertex rendering mode.
	 * @throw morda::Exc if sizes of 'p' and 't' do not match.
	 */
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec2f> t, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec2f>(i, p, t, mode);
	}
	
	/**
	 * @brief Render 3 dimensional vertices using indices.
	 * Sizes of 'p' and 't' should match.
	 * @param i - vertex indices.
	 * @param p - vertex positions.
	 * @param t - vertex texture coordinates.
	 * @param mode - vertex rendering mode.
	 * @throw morda::Exc if sizes of 'p' and 't' do not match.
	 */
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec2f> t, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec3f>(i, p, t, mode);
	}
	
private:
	template <class V> void renderInternal(const utki::Buf<V> p, const utki::Buf<kolme::Vec2f> t, Render::Mode_e mode){
		if(p.size() != t.size()){
			TRACE(<< "PosTexShader::renderInternal(): passed in array sizes do not match: p.size() = " << p.size() << " t.size() = " << t.size() << std::endl)
			throw morda::Exc("PosTexShader::renderInternal(): passed in array sizes do not match");
		}
		this->setVertexAttribArray(this->positionAttr, p);
		this->setVertexAttribArray(this->texCoordAttr, t);
		this->renderArrays(mode, p.size());
	}
	
	template <class V> void renderInternal(const utki::Buf<std::uint16_t> i, const utki::Buf<V> p, const utki::Buf<kolme::Vec2f> t, Render::Mode_e mode){
		if(p.size() != t.size()){
			TRACE(<< "PosTexShader::renderInternal(): passed in array sizes do not match: p.size() = " << p.size() << " t.size() = " << t.size() << std::endl)
			throw morda::Exc("PosTexShader::renderInternal(): passed in array sizes do not match");
		}
		this->setVertexAttribArray(this->positionAttr, p);
		this->setVertexAttribArray(this->texCoordAttr, t);
		this->renderElements(mode, i);
	}
};



}
