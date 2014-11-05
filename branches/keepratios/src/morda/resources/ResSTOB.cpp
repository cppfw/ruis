#include <stob/dom.hpp>

#include "ResSTOB.hpp"
#include "../ResourceManager.hpp"


using namespace morda;



ResSTOB::ResSTOB(std::unique_ptr<stob::Node> gui) :
		s(std::move(gui))
{}



std::shared_ptr<ResSTOB> ResSTOB::Load(const stob::Node& chain, const ting::fs::File& fi){
	fi.SetPath(chain.side("file").up().Value());
	
	return ting::New<ResSTOB>(stob::Load(fi));
}
