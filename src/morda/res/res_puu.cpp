#include <puu/dom.hpp>

#include "res_puu.hpp"
#include "../ResourceManager.hpp"


using namespace morda;

res_puu::res_puu(puu::forest&& forest) :
		s(std::move(forest))
{}

std::shared_ptr<res_puu> res_puu::load(const puu::forest& desc, const papki::file& fi){
	for(auto& p : desc){
		if(p.value == "file"){
			fi.set_path(get_property_value(p).to_string());
		}
	}

	return std::make_shared<res_puu>(puu::read(fi));
}
