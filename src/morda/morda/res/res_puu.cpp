#include "res_puu.hpp"
#include "../resource_loader.hpp"

#include "../context.hpp"

using namespace morda;

res_puu::res_puu(std::shared_ptr<morda::context> c, puu::forest&& forest) :
		resource(std::move(c)),
		s(std::move(forest))
{}

std::shared_ptr<res_puu> res_puu::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi){
	for(auto& p : desc){
		if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}

	return std::make_shared<res_puu>(utki::make_shared_from_this(ctx), puu::read(fi));
}
