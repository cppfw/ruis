#include "SizeContainer.hpp"

using namespace morda;

SizeContainer::SizeContainer(const stob::Node* chain) :
		Widget(chain),
		Container(chain)
{
}

void SizeContainer::layOut() {
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
	for(auto& w : this->children()){
		if(w->needsRelayout()){
			auto d = this->dimForWidget(*w, this->getLayoutParams(*w));
			w->resize(d);
		}
	}
}
