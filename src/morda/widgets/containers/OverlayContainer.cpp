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
