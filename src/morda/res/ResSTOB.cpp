#include <puu/dom.hpp>

#include "ResSTOB.hpp"
#include "../ResourceManager.hpp"


using namespace morda;



ResSTOB::ResSTOB(std::unique_ptr<stob::Node> gui) :
		s(std::move(gui))
{}



std::shared_ptr<ResSTOB> ResSTOB::load(const stob::Node& chain, const papki::File& fi){
	fi.setPath(chain.side("file").up().value());
	
	return std::make_shared<ResSTOB>(stob::load(fi));
}
