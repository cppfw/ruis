#include "ResTexture.hpp"

#include "../util/RasterImage.hpp"
#include "../util/util.hpp"
#include "../context.hpp"


using namespace morda;



std::shared_ptr<ResTexture> ResTexture::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi){
	for(auto& p: desc){
		if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}

	return std::make_shared<ResTexture>(ctx.shared_from_this(), loadTexture(*ctx.renderer, fi));
}
