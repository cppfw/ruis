#include "Overlay.hpp"
#include "../proxy/MouseProxy.hpp"


using namespace morda;

namespace{

const char* ContextMenuLayout_c = R"qwertyuiop(
	layout{
		dx{fill} dy{fill}
	}
	MouseProxy{
		name{morda_overlay_mouseproxy}
		layout{
			dx{fill} dy{fill}
		}
	}
	Container{
		name{morda_overlay_container}
		layout{
			dx{fill} dy{fill}
		}
	}
)qwertyuiop";

}

Overlay::Overlay(const stob::Node* chain) :
		Widget(chain),
		Pile(chain)
{
	this->onChildrenListChanged();
}

void Overlay::onChildrenListChanged(){
	if(!this->overlayLayer || !this->overlayLayer->parent()){
		this->overlayLayer = std::make_shared<Pile>(stob::parse(ContextMenuLayout_c).get());
		this->add(this->overlayLayer);
		
		this->overlayContainer = this->overlayLayer->findByNameAs<Container>("morda_overlay_container");
		ASSERT(this->overlayContainer)
		
		auto mp = this->overlayLayer->findByNameAs<MouseProxy>("morda_overlay_mouseproxy");
		ASSERT(mp)
		
		mp->mouseButton = [this](Widget& w, bool isDown, const Vec2r& pos, MouseButton_e button, unsigned id) -> bool{
			this->hideContextMenu();
			return false;
		};
	}
	
	ASSERT(this->overlayLayer)
	ASSERT(this->children().size() >= 1)
	
	if(this->children().back() != this->overlayLayer){
		auto w = this->overlayLayer->removeFromParent();
		this->add(w);
	}
}



void Overlay::showContextMenu(std::shared_ptr<Widget> w, Vec2r anchor){
	auto& lp = this->getLayoutParamsDuringLayoutAs<LayoutParams>(*w);
	
	Vec2r dim = this->dimForWidget(*w, lp);

	for(unsigned i = 0; i != 2; ++i){
		utki::clampTop(dim[i], this->overlay().rect().d[i]);
	}
	
	this->overlay().add(w);
	
	w->resize(dim);
	
	for(unsigned i = 0; i != 2; ++i){
		utki::clampRange(anchor[i], 0.0f, this->overlay().rect().d[i] - w->rect().d[i]);
	}
	
	w->moveTo(anchor);
}

void Overlay::hideContextMenu(){
	if(this->overlayContainer->children().size() == 0){
		return;
	}
	ASSERT(this->overlayContainer->children().size() > 0)
	
	this->overlayContainer->remove(*this->overlayContainer->children().back());
}
