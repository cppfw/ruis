#include "cursor.hpp"

#include "../context.hpp"

#include "../util/util.hpp"


using namespace morda::res;

cursor::cursor(std::shared_ptr<morda::context> c, res::image& image, const vector2& hotspot) :
		resource(std::move(c)),
		image_v(utki::make_shared_from_this(image)),
		hotspot_v(hotspot)
{}


std::shared_ptr<cursor> cursor::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi) {
	std::shared_ptr<res::image> image;
	vector2 hotspot;
	bool hotspot_set = false;

	for(auto& p : desc){
		if(p.value == "image"){
			image = ctx.loader.load<res::image>(get_property_value(p).to_string());
		}else if(p.value == "hotspot"){
			hotspot = parse_vec2(p.children);
			hotspot_set = true;
		}
	}

	if(!image){
		throw std::logic_error("cursor::load(): resource description does not contain 'image' property");
	}
	
	if(!hotspot_set){
		throw std::logic_error("cursor::load(): resource description does not contain 'hotspot' property");
	}
	
	return std::make_shared<cursor>(utki::make_shared_from_this(ctx), *image, hotspot);
}
