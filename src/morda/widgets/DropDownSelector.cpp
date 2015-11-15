#include "DropDownSelector.hpp"


#include "../App.hpp"

#include "Button.hpp"
#include "TextWidget.hpp"

#include "containers/OverlayContainer.hpp"

#include "labels/TextLabel.hpp"

using namespace morda;



namespace{

const char* DLayout = R"qwertyuiop(
		FrameContainer{
			name{morda_dropdown_selection}
			layout{
				dimX{0} dimY{min}
				weight{1}
			}
		}
		PushButton{
			name{morda_dropdown_button}
			TextLabel{
				text{\/}
			}
		}
	)qwertyuiop";

const char* DDropDownLayout = R"qwertyuiop(
		ColorLabel{
			layout{
				dimX{max} dimY{max}
			}
			color{0xff808080}
		}
		VerticalContainer{
			name{morda_dropdown_list}
		}
	)qwertyuiop";


class StaticProvider : public DropDownSelector::ItemsProvider{
	std::vector<std::unique_ptr<stob::Node>> widgets;
public:

	size_t count() const noexcept override{
		return this->widgets.size();
	}
	
	std::shared_ptr<Widget> getWidget(size_t index)override{
//		TRACE(<< "StaticProvider::getWidget(): index = " << index << std::endl)
		return morda::App::inst().inflater.Inflate(*(this->widgets[index]));
	}
	

	void recycle(size_t index, std::shared_ptr<Widget> w)override{
//		TRACE(<< "StaticProvider::recycle(): index = " << index << std::endl)
	}

	
	void add(std::unique_ptr<stob::Node> w){
		this->widgets.push_back(std::move(w));
	}
};

}


DropDownSelector::DropDownSelector(const stob::Node* chain) :
		Widget(chain),
		HorizontalContainer(stob::parse(DLayout).get())
{
	this->selectionContainer = this->findChildByNameAs<FrameContainer>("morda_dropdown_selection");
	ASSERT(this->selectionContainer)
	
	auto b = this->findChildByNameAs<PushButton>("morda_dropdown_button");
	b->clicked = [this](PushButton& b){
		if(!this->provider){
			return;
		}
		
		auto p = this->findAncestor<OverlayContainer>();
		if(!p){
			TRACE(<< "DropDownSelector: no OverlayContainer parent found" << std::endl)
			return;
		}
		
		this->dropDown = utki::makeShared<FrameContainer>(stob::parse(DDropDownLayout).get());
		auto lw = this->dropDown->findChildByNameAs<VerticalContainer>("morda_dropdown_list");
		ASSERT(lw)
		
		for(size_t i = 0; i != this->provider->count(); ++i){
			lw->add(this->provider->getWidget(i));
		}
		
		p->showContextMenu(this->dropDown, this->calcPosInParent(Vec2r(0), p));
	};
	
	
	if(!chain){
		return;
	}
	
	const stob::Node* n = chain->thisOrNextNonProperty().node();
	
	if(!n){
		return;
	}
	
	std::shared_ptr<StaticProvider> p = utki::makeShared<StaticProvider>();
	
	for(; n; n = n->nextNonProperty().node()){
		p->add(n->clone());
	}
	
	this->setItemsProvider(std::move(p));
}

void DropDownSelector::setItemsProvider(std::shared_ptr<ItemsProvider> provider){
	if(provider && provider->dd){
		throw Exc("given provider is already set to some DropDownSelector");
	}
	
	if(this->provider){
		this->provider->dd = nullptr;
	}
	this->provider = std::move(provider);
	if(this->provider){
		this->provider->dd = this;
	}
	this->handleDataSetChanged();
}

void DropDownSelector::ItemsProvider::notifyDataSetChanged(){
	if(this->dd){
		this->dd->handleDataSetChanged();
	}
}

void DropDownSelector::handleDataSetChanged(){
	this->selectionContainer->removeAll();
	
	if(!this->provider){
		return;
	}
	if(this->selectedItem >= this->provider->count()){
		return;
	}
	
	this->selectionContainer->add(this->provider->getWidget(this->selectedItem));
}
