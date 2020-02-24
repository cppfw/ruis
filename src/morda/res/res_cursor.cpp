#include "res_cursor.hpp"

#include "../context.hpp"

#include "../util/util.hpp"


using namespace morda;

res_cursor::res_cursor(std::shared_ptr<morda::context> c, res_image& image, const Vec2r& hotspot) :
		resource(std::move(c)),
		image_v(std::dynamic_pointer_cast<res_image>(image.shared_from_this())),
		hotspot_v(hotspot)
{}


std::shared_ptr<res_cursor> res_cursor::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi) {
	std::shared_ptr<res_image> image;
	Vec2r hotspot;
	bool hotspot_set = false;

	for(auto& p : desc){
		if(p.value == "image"){
			image = ctx.loader.load<res_image>(get_property_value(p).to_string());
		}else if(p.value == "hotspot"){
			hotspot = parse_vec2(p.children);
			hotspot_set = true;
		}
	}

	if(!image){
		throw std::logic_error("res_cursor::load(): resource description does not contain 'image' property");
	}
	
	if(!hotspot_set){
		throw std::logic_error("res_cursor::load(): resource description does not contain 'hotspot' property");
	}
	
	return std::make_shared<res_cursor>(ctx.shared_from_this(), *image, hotspot);
}
