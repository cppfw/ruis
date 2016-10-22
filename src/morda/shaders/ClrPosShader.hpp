#pragma once

#include <utki/Buf.hpp>

#include "PosShader.hpp"

namespace morda{

/**
 * @brief Shader with color and position vertex attributes.
 * This shader has color and position vertex attributes.
 */
class PosClrShader  :
		protected PosShader,
		virtual public Shader
{
	Render::InputID colorAttr;
	
public:
	PosClrShader();
	
	PosClrShader(const PosClrShader&) = delete;
	PosClrShader& operator=(const PosClrShader&) = delete;
	
	/**
	 * @brief Render array of 2 dimensional vertices.
	 * The sizes of vertex positions and vertex colors buffers should match.
	 * @param p - array of vertex positions.
	 * @param c - array of vertex colors.
	 * @param mode - vertex rendering mode.
	 * @throw morda::Exc if size of 'p' does not match size of 'c'.
	 */
	void render(const utki::Buf<kolme::Vec2f> p, const utki::Buf<std::uint32_t> c, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal(p, c, mode);
	}
	
	/**
	 * @brief Render array of 2 dimensional vertices.
	 * The sizes of vertex positions and vertex colors buffers should match.
	 * @param p - array of vertex positions.
	 * @param c - array of vertex colors.
	 * @param mode - vertex rendering mode.
	 * @throw morda::Exc if size of 'p' does not match size of 'c'.
	 */
	void render(const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec4f> c, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal(p, c, mode);
	}
	
	/**
	 * @brief Render array of 3 dimensional vertices.
	 * The sizes of vertex positions and vertex colors buffers should match.
	 * @param p - array of vertex positions.
	 * @param c - array of vertex colors.
	 * @param mode - vertex rendering mode.
	 * @throw morda::Exc if size of 'p' does not match size of 'c'.
	 */
	void render(const utki::Buf<kolme::Vec3f> p, const utki::Buf<kolme::Vec4f> c, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec3f>(p, c, mode);
	}
	
	/**
	 * @brief Render 2 dimensional vertices using indices.
	 * The sizes of vertex positions and vertex colors buffers should match.
	 * @param i - array of vertex indices.
	 * @param p - array of vertex positions.
	 * @param c - array of vertex colors.
	 * @param mode - vertex rendering mode.
	 * @throw morda::Exc if size of 'p' does not match size of 'c'.
	 */
	void render(const utki::Buf<std::uint16_t> i, const utki::Buf<kolme::Vec2f> p, const utki::Buf<kolme::Vec4f> c, Render::Mode_e mode = Render::Mode_e::TRIANGLE_FAN){
		this->renderInternal<kolme::Vec2f>(i, p, c, mode);
	}
	
	/**
	 * @brief Render 3 dimensional vertices using indices.
	 * The sizes of vertex positions and vertex colors buffers should match.
	 * @param i - array of vertex indices.
	 * @param p - array of vertex positions.
	 * @param c - array of vertex colors.
	 * @param mode - vertex rendering mode.
	 * @throw morda::Exc if size of 'p' does not match size of 'c'.
	 */
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
