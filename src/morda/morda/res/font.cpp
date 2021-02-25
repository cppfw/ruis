#include "font.hpp"

#include "../context.hpp"

#include "../util/util.hpp"

#include "../fonts/texture_font.hxx"

#include <utki/unicode.hpp>

#include <memory>

using namespace morda;
using namespace morda::res;

res::font::font(
		std::shared_ptr<morda::context> context,
		const papki::file& file_normal,
		std::unique_ptr<const papki::file> file_bold,
		std::unique_ptr<const papki::file> file_italic,
		std::unique_ptr<const papki::file> file_bold_italic,
		unsigned font_size,
		unsigned max_cached
	) :
		resource(std::move(context))
{
	this->fonts[unsigned(style::normal)] = std::make_unique<texture_font>(this->context, file_normal, font_size, max_cached);

	if(file_bold){
		this->fonts[unsigned(style::bold)] = std::make_unique<texture_font>(this->context, *file_bold, font_size, max_cached);
	}
	if(file_italic){
		this->fonts[unsigned(style::italic)] = std::make_unique<texture_font>(this->context, *file_italic, font_size, max_cached);
	}
	if(file_bold_italic){
		this->fonts[unsigned(style::bold_italic)] = std::make_unique<texture_font>(this->context, *file_bold_italic, font_size, max_cached);
	}
}

std::shared_ptr<res::font> res::font::load(morda::context& ctx, const treeml::forest& desc, const papki::file& fi){
	unsigned fontSize = 13;
	unsigned maxCached = unsigned(-1);

	std::unique_ptr<const papki::file> file_bold;
	std::unique_ptr<const papki::file> file_italic;
	std::unique_ptr<const papki::file> file_bold_italic;

	for(auto& p : desc){
		if(p.value == "size"){
			fontSize = unsigned(parse_dimension_value(get_property_value(p), ctx.units));
		}else if(p.value == "max_cached"){
			maxCached = unsigned(get_property_value(p).to_uint32());
		}else if(p.value == "normal"){
			fi.set_path(get_property_value(p).to_string());
		}else if(p.value == "bold"){
			file_bold = fi.spawn(get_property_value(p).to_string());
		}else if(p.value == "italic"){
			file_italic = fi.spawn(get_property_value(p).to_string());
		}else if(p.value == "bold_italic"){
			file_bold_italic = fi.spawn(get_property_value(p).to_string());
		}
	}

	return std::make_shared<font>(
			utki::make_shared_from(ctx),
			fi,
			std::move(file_bold),
			std::move(file_italic),
			std::move(file_bold_italic),
			fontSize,
			maxCached
		);
}
