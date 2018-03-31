#pragma once

#include <string>

#include <stob/dom.hpp>

#include "../ResourceManager.hpp"

#include "../fonts/TexFont.hpp"



namespace morda{



/**
 * @brief %Font resource.
 * 
 * %Resource description:
 * 
 * @param file - file to load the font from, TrueType ttf file.
 * @param chars - list of all chars for which the glyphs should be created.
 * @param size - size of glyphs, in length units, i.e.: no unit(pixels), pt, mm.
 * @param outline - thickness of the outline in length units.
 * 
 * Example:
 * @code
 * fnt_normal{
 *     file {Vera.ttf}
 *     chars {" 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,:;?!()_*+-|\\/"}
 *     size {12pt}
 *     outline {0}
 * }
 * @endcode
 */
class ResFont : public morda::Resource{
	friend class morda::ResourceManager;

	morda::TexFont f;

public:
	ResFont(const papki::File& fi, const std::u32string& chars, unsigned fontSize);

	~ResFont()noexcept{}

	/**
	 * @brief Get font object held by this resource.
	 * @return Font object.
	 */
	const morda::Font& font()noexcept{
		return this->f;
	}
	
private:
	static std::shared_ptr<ResFont> load(const stob::Node& chain, const papki::File &fi);
};



}//~namespace
