#include "res_font.hpp"

#include "../context.hpp"

#include "../util/util.hpp"

#include "../fonts/texture_font.hxx"

#include <unikod/utf8.hpp>

#include <memory>

using namespace morda;



res_font::res_font(std::shared_ptr<morda::context> c, const papki::file& fi, unsigned fontSize, unsigned maxCached) :
		resource(std::move(c)),
		f(std::make_unique<texture_font>(this->context, fi, fontSize, maxCached))
{}



std::shared_ptr<res_font> res_font::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi){
	unsigned fontSize = 13;
	unsigned maxCached = unsigned(-1);

	for(auto& p : desc){
		if(p.value == "size"){
			fontSize = unsigned(parse_dimension_value(get_property_value(p), ctx.units));
		}else if(p.value == "maxCached"){
			maxCached = unsigned(get_property_value(p).to_uint32());
		}else if(p.value == "file"){
			fi.setPath(get_property_value(p).to_string());
		}
	}

	return std::make_shared<res_font>(ctx.shared_from_this(), fi, fontSize, maxCached);
}

