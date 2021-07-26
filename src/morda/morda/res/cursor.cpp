/*
morda GUI framework
Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "cursor.hpp"

#include "../context.hpp"

#include "../util/util.hpp"


using namespace morda::res;

morda::res::cursor::cursor(std::shared_ptr<morda::context> c, morda::res::image& image, const vector2& hotspot) :
		resource(std::move(c)),
		image_v(utki::make_shared_from(image)),
		hotspot_v(hotspot)
{}


std::shared_ptr<cursor> cursor::load(morda::context& ctx, const treeml::forest& desc, const papki::file& fi) {
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
	
	return std::make_shared<cursor>(utki::make_shared_from(ctx), *image, hotspot);
}
