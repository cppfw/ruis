/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "../ResourceManager.hpp"


namespace morda{

class ResSTOB : public Resource{
	friend class ResourceManager;
	
	std::unique_ptr<stob::Node> s;
public:
	ResSTOB(std::unique_ptr<stob::Node> gui);
	
	ResSTOB(const ResSTOB&) = delete;
	ResSTOB& operator=(const ResSTOB&) = delete;
	
	const stob::Node* chain()const{
		return this->s.get();
	}
	
private:
	static std::shared_ptr<ResSTOB> load(const stob::Node& chain, const papki::File& fi);
};

}
