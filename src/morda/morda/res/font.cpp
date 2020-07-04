#include "font.hpp"

#include "../context.hpp"

#include "../util/util.hpp"

#include "../fonts/texture_font.hxx"

#include <utki/utf8.hpp>

#include <memory>

using namespace morda;
using namespace morda::res;

res::font::font(std::shared_ptr<morda::context> c, const papki::file& fi, unsigned fontSize, unsigned maxCached) :
		resource(std::move(c)),
		f(std::make_unique<texture_font>(this->context, fi, fontSize, maxCached))
{}

std::shared_ptr<res::font> res::font::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi){
	unsigned fontSize = 13;
	unsigned maxCached = unsigned(-1);

	for(auto& p : desc){
		if(p.value == "size"){
			fontSize = unsigned(parse_dimension_value(get_property_value(p), ctx.units));
		}else if(p.value == "maxCached"){
			maxCached = unsigned(get_property_value(p).to_uint32());
		}else if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}

	return std::make_shared<font>(utki::make_shared_from_this(ctx), fi, fontSize, maxCached);
}

