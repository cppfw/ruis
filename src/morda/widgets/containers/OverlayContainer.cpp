#include "OverlayContainer.hpp"


using namespace morda;



OverlayContainer::OverlayContainer(const stob::Node* chain) :
		Widget(chain),
		FrameContainer(chain)
{
	this->onChildrenListChanged();
}

void OverlayContainer::onChildrenListChanged(){
	if(!this->overlayContainer || !this->overlayContainer->parent()){
		this->overlayContainer = utki::makeShared<Container>();
		this->add(this->overlayContainer);

		auto& lp = this->getLayoutParams(*this->overlayContainer);

		lp.dim.y = Widget::LayoutParams::D_Max;
		lp.dim.x = Widget::LayoutParams::D_Max;
	}
	
	ASSERT(this->overlayContainer)
	ASSERT(this->children().size() >= 1)
	
	if(this->children().back() != this->overlayContainer){
		auto w = this->overlayContainer->removeFromParent();
		this->add(w);
	}
}



void OverlayContainer::showContextMenu(std::shared_ptr<Widget> w, Vec2r anchor){
	auto& lp = this->getLayoutParamsAs<LayoutParams>(*w);
	
	Vec2r dim = this->dimForWidget(*w, lp);

	for(unsigned i = 0; i != 2; ++i){
		utki::clampTop(dim[i], this->overlay().rect().d[i]);
	}
	
	this->overlay().add(w);
	
	w->resize(dim);
	
	anchor.y -= dim.y;
	
	for(unsigned i = 0; i != 2; ++i){
		utki::clampRange(anchor[i], 0.0f, this->overlay().rect().d[i] - w->rect().d[i]);
	}
	
	w->moveTo(anchor);
}
