#include "ResTexture.hpp"

#include "../exception.hpp"
#include "../util/RasterImage.hpp"
#include "../util/util.hpp"



using namespace morda;



std::shared_ptr<ResTexture> ResTexture::load(const puu::forest& desc, const papki::file& fi){
	for(auto& p: desc){
		if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}

	return std::make_shared<ResTexture>(loadTexture(fi));
}
