#include "treeml.hpp"
#include "../resource_loader.hpp"

#include "../context.hpp"

using namespace morda::res;

morda::res::treeml::treeml(std::shared_ptr<morda::context> c, ::treeml::forest&& forest) :
		resource(std::move(c)),
		s(std::move(forest))
{}

std::shared_ptr<morda::res::treeml> morda::res::treeml::load(morda::context& ctx, const ::treeml::forest& desc, const papki::file& fi){
	for(auto& p : desc){
		if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}

	return std::make_shared<treeml>(utki::make_shared_from(ctx), ::treeml::read(fi));
}
