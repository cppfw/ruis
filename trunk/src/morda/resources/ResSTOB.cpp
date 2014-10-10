#include <stob/dom.hpp>

#include "ResSTOB.hpp"
#include "../ResourceManager.hpp"


using namespace morda;



ResSTOB::ResSTOB(std::unique_ptr<stob::Node> gui) :
		s(std::move(gui))
{}



std::shared_ptr<ResSTOB> ResSTOB::Load(const stob::Node& r, const ting::fs::File& fi){
	auto fileName = r.GetProperty("file");
	if(!fileName){
		throw ResourceManager::Exc("ResGui::Load(): 'file' property not found in resource description");
	}
	
	fi.SetPath(fileName->Value());
	
	return ting::New<ResSTOB>(stob::Load(fi));
}
