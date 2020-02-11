#include "ResFont.hpp"

#include "../exception.hpp"

#include "../context.hpp"

#include "../util/util.hpp"

#include "../fonts/TexFont.hxx"

#include <unikod/utf8.hpp>

using namespace morda;



ResFont::ResFont(const papki::file& fi, unsigned fontSize, unsigned maxCached) :
		f(utki::make_unique<TexFont>(fi, fontSize, maxCached))
{}



std::shared_ptr<ResFont> ResFont::load(gui& ctx, const puu::forest& desc, const papki::file& fi){
	unsigned fontSize = 13;
	unsigned maxCached = unsigned(-1);

	for(auto& p : desc){
		if(p.value == "size"){
			fontSize = unsigned(parse_dimension_value(get_property_value(p)));
		}else if(p.value == "maxCached"){
			maxCached = unsigned(get_property_value(p).to_uint32());
		}else if(p.value == "file"){
			fi.setPath(get_property_value(p).to_string());
		}
	}

	return std::make_shared<ResFont>(fi, fontSize, maxCached);
}

