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
		FrameContainer(stob::Parse(DDesc).get())
{
	this->contentContainer = this->FindChildByNameAs<FrameContainer>("morda_content");
	this->overlayContainer = this->FindChildByNameAs<Container>("morda_overlay");
	
	if(chain){
		ASSERT(this->contentContainer);
		this->contentContainer->Add(*chain);
	}
}
