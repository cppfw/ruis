#pragma once

#include <string>

#include <puu/dom.hpp>

#include "../ResourceManager.hpp"

#include "../fonts/Font.hpp"


namespace morda{



/**
 * @brief %Font resource.
 * 
 * %Resource description:
 * 
 * @param file - file to load the font from, TrueType ttf file.
 * @param chars - list of all chars for which the glyphs should be created.
 * @param size - size of glyphs, in length units, i.e.: no unit(pixels), dp, mm.
 * @param outline - thickness of the outline in length units.
 * 
 * Example:
 * @code
 * fnt_normal{
 *     file {Vera.ttf}
 *     size {12dp}
 * }
 * @endcode
 */
class ResFont : public morda::Resource{
	friend class morda::ResourceManager;

	std::unique_ptr<morda::Font> f;

public:
	ResFont(const papki::File& fi, unsigned fontSize, unsigned maxCached);

	~ResFont()noexcept{}

	/**
	 * @brief Get font object held by this resource.
	 * @return Font object.
	 */
	const morda::Font& font()noexcept{
		ASSERT(this->f)
		return *this->f;
	}
	
private:
	static std::shared_ptr<ResFont> load(const stob::Node& chain, const papki::File &fi);
};



}//~namespace
