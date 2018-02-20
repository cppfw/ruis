#include "DropDownSelector.hpp"

#include "../Morda.hpp"

#include "core/container/Overlay.hpp"

#include "proxy/MouseProxy.hpp"

#include "button/Button.hpp"

#include "label/Text.hpp"
#include "label/Color.hpp"

#include "base/TextWidget.hpp"
#include "label/NinePatch.hpp"



using namespace morda;



namespace{

const auto selectorLayout_c = stob::parse(R"qwertyuiop(
	layout{dx{max} dy{max}}

	Row{
		layout{dx{max}}
		Pile{
			name{morda_dropdown_selection}
			layout{dx{min}dy{max} weight{1}}
		}
		Widget{layout{dx{3pt}}}
		Image{
			image{morda_img_divider_vert}
			layout{dy{fill}}
		}
		Widget{layout{dx{3pt}}}
		Image{
			image{morda_img_dropdown_arrow}
		}
	}
)qwertyuiop");

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
			Color{
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
			Column{
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

	auto va = np->findByNameAs<morda::Column>("morda_contextmenu_content");
	ASSERT(va)

	for(size_t i = 0; i != this->provider->count(); ++i){
		va->add(this->wrapItem(this->provider->getWidget(i), i));
	}

	overlay->showContextMenu(np, this->calcPosInParent(Vec2r(0), overlay) + Vec2r(0, this->rect().d.y));
}


DropDownSelector::DropDownSelector(const stob::Node* chain) :
		Widget(chain),
		Button(selectorLayout_c.get()),
		NinePatchPushButton(selectorLayout_c.get()),
		selectionContainer(*this->findByNameAs<Pile>("morda_dropdown_selection"))
{
	this->pressedChanged = [this](Button& b){
		if(!b.isPressed()){
			return;
		}

		this->showDropdownMenu();
	};
	
	if(!chain){
		return;
	}
	
	const stob::Node* n = chain->thisOrNextNonProperty().node();
	
	if(!n){
		return;
	}
	
	std::shared_ptr<StaticProvider> p = std::make_shared<StaticProvider>();
	
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

	auto mp = wd->findByNameAs<MouseProxy>("morda_dropdown_mouseproxy");
	ASSERT(mp)

	auto cl = wd->findByNameAs<Color>("morda_dropdown_color");
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
			morda::Morda::inst().postToUiThread([oc](){
				oc->hideContextMenu();
			});
		}

		return true;
	};

	return wd;
}

//bool DropDownSelector::onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId) {
//	if(button != MouseButton_e::LEFT){
//		return false;
//	}
//	if(!isDown){
//		return false;
//	}
//	
//	if(this->isPressed()){
//		this->showDropdownMenu();
//		return true;
//	}
//	
//	return this->SimpleButton::onMouseButton(isDown, pos, button, pointerId);
//}
//
//void DropDownSelector::onHoverChanged(unsigned pointerID) {
//
//}
