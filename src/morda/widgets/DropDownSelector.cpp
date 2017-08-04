#include "DropDownSelector.hpp"

#include "../Morda.hpp"

#include "core/container/Overlay.hpp"

#include "core/proxy/MouseProxy.hpp"

#include "button/Button.hpp"

#include "label/TextLabel.hpp"
#include "label/ColorLabel.hpp"

#include "TextWidget.hpp"
#include "NinePatch.hpp"



using namespace morda;



namespace{

const char* selectorLayout_c = R"qwertyuiop(
		Pile{
			layout{
				dx{0}
				weight{1}
			}
			NinePatch{
				layout{dx{fill}dy{max}}
				image{morda_npt_dropdown_selector_bg}

				Pile{
					name{morda_dropdown_selection}
					layout{dx{fill}dy{max}}
				}
			}
			MouseProxy{
				name{morda_dropdown_mouseproxy}
				layout{dx{fill}dy{fill}}
			}
		}
		PushButton{
			name{morda_dropdown_button}

			layout{dy{max}}

			look{
				imageNormal{morda_npt_rightbutton_normal}
				imagePressed{morda_npt_rightbutton_pressed}
			}

			ImageLabel{
				image{morda_img_dropdown_arrow}
			}
		}
	)qwertyuiop";

const char* itemLayout_c = R"qwertyuiop(
		Pile{
			layout{
				dx{max}
			}
			MouseProxy{
				name{morda_dropdown_mouseproxy}
				layout{
					dx{max} dy{max}
				}
			}
			ColorLabel{
				name{morda_dropdown_color}
				color{@{morda_color_highlight}}
				layout{
					dx{max} dy{max}
				}
			}
		}
	)qwertyuiop";

const char* contextMenuLayout_c = R"qwertyuiop(
		NinePatch{
			image{morda_npt_contextmenu_bg}
			VerticalArea{
				name{morda_contextmenu_content}
			}
		}
	)qwertyuiop";


class StaticProvider : public DropDownSelector::ItemsProvider{
	std::vector<std::unique_ptr<stob::Node>> widgets;
public:

	size_t count() const noexcept override{
		return this->widgets.size();
	}
	
	std::shared_ptr<Widget> getWidget(size_t index)override{
		return morda::Morda::inst().inflater.inflate(*(this->widgets[index]));
	}
	

	void recycle(size_t index, std::shared_ptr<Widget> w)override{
//		TRACE(<< "StaticProvider::recycle(): index = " << index << std::endl)
	}

	
	void add(std::unique_ptr<stob::Node> w){
		this->widgets.push_back(std::move(w));
	}
};

}

void DropDownSelector::showDropdownMenu() {
	if(!this->provider){
		return;
	}

	auto overlay = this->findAncestor<Overlay>();
	if(!overlay){
		throw Exc("DropDownSelector: no Overlay parent found");
	}

	auto np = morda::Morda::inst().inflater.inflate(*stob::parse(contextMenuLayout_c));
	ASSERT(np)

	auto va = np->findChildByNameAs<VerticalArea>("morda_contextmenu_content");
	ASSERT(va)

	for(size_t i = 0; i != this->provider->count(); ++i){
		va->add(this->wrapItem(this->provider->getWidget(i), i));
	}

	overlay->showContextMenu(np, this->calcPosInParent(Vec2r(0), overlay));
}



DropDownSelector::DropDownSelector(const stob::Node* chain) :
		Widget(chain),
		HorizontalArea(stob::parse(selectorLayout_c).get()),
		selectionContainer(*this->findChildByNameAs<Pile>("morda_dropdown_selection"))
{	
	{
		auto b = this->findChildByNameAs<PushButton>("morda_dropdown_button");
		ASSERT(b)
		b->pressedChanged = [this](Button& b){
			if(!b.isPressed()){
				return;
			}

			this->showDropdownMenu();
		};
	}
	{
		auto mp = this->findChildByNameAs<MouseProxy>("morda_dropdown_mouseproxy");
		ASSERT(mp)
		mp->mouseButton = [this](Widget& widget, bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId) -> bool{
			if(!isDown){
				return false;
			}
			
			this->showDropdownMenu();
			
			return true;
		};
	}
	
	
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
		throw Exc("DropDownSelector::setItemsProvider(): given provider is already set to some DropDownSelector");
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
	this->selectionContainer.removeAll();
	
	if(!this->provider){
		return;
	}
	if(this->selectedItem_v >= this->provider->count()){
		return;
	}
	
	this->selectionContainer.add(this->provider->getWidget(this->selectedItem_v));
}

void DropDownSelector::setSelection(size_t i){
	this->selectedItem_v = i;
	
	this->handleDataSetChanged();
}

std::shared_ptr<Widget> DropDownSelector::wrapItem(std::shared_ptr<Widget>&& w, size_t index) {
	auto wd = std::dynamic_pointer_cast<Pile>(morda::Morda::inst().inflater.inflate(*stob::parse(itemLayout_c)));
	ASSERT(wd)

	auto mp = wd->findChildByNameAs<MouseProxy>("morda_dropdown_mouseproxy");
	ASSERT(mp)

	auto cl = wd->findChildByNameAs<ColorLabel>("morda_dropdown_color");
	ASSERT(cl)
	auto clWeak = utki::makeWeak(cl);

	wd->add(w);

	mp->hoverChanged = [clWeak](Widget& w, unsigned id){
		if(auto c = clWeak.lock()){
			c->setVisible(w.isHovered());
		}
	};
	mp->hoverChanged(*mp, 0);

	mp->mouseButton = [this, index](Widget& w, bool isDown, const Vec2r pos, MouseButton_e button, unsigned id) -> bool{
		if(!isDown){
			this->setSelection(index);
			auto oc = this->findAncestor<Overlay>();
			if(!oc){
				throw Exc("No Overlay found in ancestors of DropDownSelector");
			}
			morda::Morda::inst().postToUiThread_ts([oc](){
				oc->hideContextMenu();
			});
		}

		return true;
	};

	return wd;
}
