/* 
 * File:   ResSvg.hpp
 * Author: ivan
 *
 * Created on April 13, 2016, 2:10 PM
 */

#pragma once

#include "../ResourceManager.hpp"

namespace morda{

class ResSvg : public Resource{
	friend class ResourceManager;
	
public:
	
	
	ResSvg(const ResSvg&) = delete;
	ResSvg& operator=(const ResSvg&) = delete;
	
private:
	static std::shared_ptr<ResSTOB> load(const stob::Node& chain, const papki::File& fi);
};

}
