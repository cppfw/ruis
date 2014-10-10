#include <stob/dom.hpp>

#include "ResGui.hpp"
#include "../ResourceManager.hpp"


using namespace morda;



ResGui::ResGui(std::unique_ptr<stob::Node> gui) :
		gui(std::move(gui))
{}



std::shared_ptr<ResGui> ResGui::Load(const stob::Node& r, const ting::fs::File& fi){
	auto fileName = r.GetProperty("file");
	if(!fileName){
		throw ResourceManager::Exc("ResGui::Load(): 'file' property not found in resource description");
	}
	
	fi.SetPath(fileName->Value());
	
	return ting::New<ResGui>(stob::Load(fi));
}
