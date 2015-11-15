#include "OverlayContainer.hpp"


using namespace morda;



namespace{

const char* DDesc = R"qwertyuiop(
		FrameContainer{
			name{morda_content}
			layout{
				dimX{max} dimY{max}
			}
		}

		Container{
			name{morda_overlay}
			layout{
				dimX{max} dimY{max}
			}
		}
	)qwertyuiop";

}


OverlayContainer::OverlayContainer(const stob::Node* chain) :
		Widget(chain),
		FrameContainer(stob::parse(DDesc).get())
{
	this->contentContainer = this->findChildByNameAs<FrameContainer>("morda_content");
	this->overlayContainer = this->findChildByNameAs<Container>("morda_overlay");
	
	if(chain){
		ASSERT(this->contentContainer);
		this->contentContainer->add(*chain);
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
