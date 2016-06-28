#include "DropDownSelector.hpp"


#include "../App.hpp"

#include "button/Button.hpp"
#include "TextWidget.hpp"

#include "core/container/OverlayContainer.hpp"

#include "label/TextLabel.hpp"
#include "core/proxy/MouseProxy.hpp"
#include "label/ColorLabel.hpp"

using namespace morda;



namespace{

const char* DLayout = R"qwertyuiop(
		Frame{
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

const char* DDropDownItemLayout = R"qwertyuiop(
		Frame{
			layout{
				dimX{max}
			}
			MouseProxy{
				name{morda_dropdown_mouseproxy}
				layout{
					dimX{max} dimY{max}
				}
			}
			ColorLabel{
				name{morda_dropdown_color}
				layout{
					dimX{max} dimY{max}
				}
			}
		}
	)qwertyuiop";


class StaticProvider : public DropDownSelector::ItemsProvider{
	std::vector<std::unique_ptr<stob::Node>> widgets;
public:

	size_t count() const noexcept override{
		return this->widgets.size();
	}
	
	std::shared_ptr<Widget> getWidget(size_t index, bool isSelection)override{
		auto w = morda::App::inst().inflater.inflate(*(this->widgets[index]));
		
		if(isSelection){
			return w;
		}
		
		auto wd = std::dynamic_pointer_cast<Frame>(morda::App::inst().inflater.inflate(*stob::parse(DDropDownItemLayout)));
		ASSERT(wd)
		
		auto mp = wd->findChildByNameAs<MouseProxy>("morda_dropdown_mouseproxy");
		ASSERT(mp)
		
		auto cl = wd->findChildByNameAs<ColorLabel>("morda_dropdown_color");
		ASSERT(cl)
		auto clWeak = utki::makeWeak(cl);
		
		wd->add(w);
		
		mp->hoverChanged = [clWeak](Widget& w, unsigned id){
			if(auto c = clWeak.lock()){
				if(w.isHovered()){
					c->setColor(0xff800000);
				}else{
					c->setColor(0xff404040);
				}
			}
		};
		mp->hoverChanged(*mp, 0);
		
		mp->mouseButton = [this, index](Widget& w, bool isDown, const Vec2r pos, Widget::EMouseButton button, unsigned id) -> bool{
			if(!isDown){
				if(this->dropDownSelector()){
					this->dropDownSelector()->setSelection(index);
					auto oc = this->dropDownSelector()->findAncestor<OverlayContainer>();
					if(!oc){
						throw Exc("No OverlayContainer found in ancestors of DropDownSelector");
					}
					morda::App::inst().postToUiThread_ts([oc](){
						oc->hideContextMenu();
					});
				}
			}
			
			return true;
		};
		
		return wd;
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
	this->selectionContainer = this->findChildByNameAs<Frame>("morda_dropdown_selection");
	ASSERT(this->selectionContainer)
	
	auto b = this->findChildByNameAs<PushButton>("morda_dropdown_button");
	b->clicked = [this](PushButton& b){
		if(!this->provider){
			return;
		}
		
		auto p = this->findAncestor<OverlayContainer>();
		if(!p){
			throw Exc("DropDownSelector: no OverlayContainer parent found");
		}
		
		auto w = utki::makeShared<VerticalContainer>();
		
		for(size_t i = 0; i != this->provider->count(); ++i){
			w->add(this->provider->getWidget(i, false));
		}
		
		p->showContextMenu(w, this->calcPosInParent(Vec2r(0), p));
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
	
	this->selectionContainer->add(this->provider->getWidget(this->selectedItem, true));
}

void DropDownSelector::setSelection(size_t i){
	this->selectedItem = i;
	
	this->handleDataSetChanged();
}
