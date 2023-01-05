/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

#include <papki/root_dir.hpp>
#include <papki/util.hpp>

#include "resource_loader.hpp"

#include "util/util.hpp"

using namespace morda;

namespace{
const char* wording_include = "include";
const char* wording_include_subdirs = "include_subdirs";
}

void resource_loader::mount_res_pack(const papki::file& fi){
	ASSERT(!fi.is_open())
	
	std::string dir = fi.dir();
	
	if(fi.not_dir().size() == 0){
		fi.set_path(dir + "main.res");
	}

	auto script = treeml::read(fi);
	ASSERT(!fi.is_open())

	// handle includes
	for(auto& p : script){
		if(p.value == wording_include){
			fi.set_path(dir + get_property_value(p).to_string());
			this->mount_res_pack(fi);
			// TODO: remove "include" tree from the forest?
		}else if(p.value == wording_include_subdirs){
			fi.set_path(fi.dir());
			for(auto& f : fi.list_dir()){
				if(papki::is_dir(f)){
					fi.set_path(dir + f);
					this->mount_res_pack(fi);
				}
			}
			// TODO: remove "include_subdirs" tree from the forest?
		}
	}
	
	res_pack_entry rpe;
	rpe.fi = papki::root_dir::make(fi.spawn(), dir);
	rpe.script = std::move(script);

	this->resPacks.push_back(std::move(rpe));
	ASSERT(this->resPacks.back().fi)
	ASSERT(!this->resPacks.back().script.empty())
}

resource_loader::find_in_script_result resource_loader::find_resource_in_script(const std::string& resName){
	for(auto i = this->resPacks.rbegin(); i != this->resPacks.rend(); ++i){
		auto j = std::find(i->script.begin(), i->script.end(), resName);
		if(j != i->script.end()){
			return find_in_script_result(*i, *j);
		}
	}
	LOG([&](auto&o){o << "resource name not found in mounted resource packs: " << resName << std::endl;})
	std::stringstream ss;
	ss << "resource name not found in mounted resource packs: " << resName;
	throw std::logic_error(ss.str());
}

void resource_loader::add_resource(const utki::shared_ref<resource>& res, const std::string& name){
	ASSERT(this->res_map.find(name) == this->res_map.end())
	
	//add the resource to the resources map of ResMan
	this->res_map.insert(
			std::make_pair(name, utki::make_weak(res))
		);
	
//#ifdef DEBUG
//	for(T_ResMap::iterator i = this->res_map->rm.begin(); i != this->res_map->rm.end(); ++i){
//		TRACE(<< "\t" << *(*i).first << std::endl)
//	}
//#endif
}
