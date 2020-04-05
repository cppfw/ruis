#pragma once

#include <string>

#include <puu/tree.hpp>

#include "../resource_loader.hpp"

#include "../fonts/font.hpp"

#include "../util/util.hpp"

namespace morda{

/**
 * @brief %Font resource.
 * 
 * %resource description:
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
class res_font : public morda::resource{
	friend class morda::resource_loader;

	std::unique_ptr<morda::font> f;

public:
	res_font(std::shared_ptr<morda::context> context, const papki::file& fi, unsigned fontSize, unsigned maxCached);

	~res_font()noexcept{}

	/**
	 * @brief Get font object held by this resource.
	 * @return Font object.
	 */
	const morda::font& font()noexcept{
		ASSERT(this->f)
		return *this->f;
	}
	
private:
	static std::shared_ptr<res_font> load(morda::context& ctx, const puu::forest& desc, const papki::file &fi);
};



}
