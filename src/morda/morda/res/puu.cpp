#include "puu.hpp"
#include "../resource_loader.hpp"

#include "../context.hpp"

using namespace morda::res;

morda::res::puu::puu(std::shared_ptr<morda::context> c, ::puu::forest&& forest) :
		resource(std::move(c)),
		s(std::move(forest))
{}

std::shared_ptr<morda::res::puu> morda::res::puu::load(morda::context& ctx, const ::puu::forest& desc, const papki::file& fi){
	for(auto& p : desc){
		if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}

	return std::make_shared<puu>(utki::make_shared_from_this(ctx), ::puu::read(fi));
}
