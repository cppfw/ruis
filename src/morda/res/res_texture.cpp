#include "res_texture.hpp"

#include "../util/RasterImage.hpp"
#include "../util/util.hpp"
#include "../context.hpp"


using namespace morda;



std::shared_ptr<res_texture> res_texture::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi){
	for(auto& p: desc){
		if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}

	return std::make_shared<res_texture>(ctx.shared_from_this(), load_texture(*ctx.renderer, fi));
}
