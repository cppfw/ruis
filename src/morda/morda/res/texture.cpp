#include "texture.hpp"

#include "../util/raster_image.hpp"
#include "../util/util.hpp"
#include "../context.hpp"

using namespace morda;
using namespace morda::res;

std::shared_ptr<texture> texture::load(morda::context& ctx, const treeml::forest& desc, const papki::file& fi){
	for(auto& p: desc){
		if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}

	return std::make_shared<texture>(utki::make_shared_from(ctx), load_texture(*ctx.renderer, fi));
}
