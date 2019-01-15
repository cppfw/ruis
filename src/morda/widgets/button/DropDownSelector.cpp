#include "DropDownSelector.hpp"

#include "../../Morda.hpp"

#include "../group/Overlay.hpp"

#include "../proxy/MouseProxy.hpp"

#include "../button/Button.hpp"

#include "../label/Text.hpp"
#include "../label/Color.hpp"
#include "../label/NinePatch.hpp"

#include "../base/TextWidget.hpp"




using namespace morda;



namespace{

const auto selectorLayout_c = stob::parse(R"qwertyuiop(
	layout{dx{max} dy{max}}

	Row{
		layout{dx{max}}
		Pile{
			id{morda_dropdown_selection}
			layout{dx{min}dy{max} weight{1}}
		}
		Widget{layout{dx{3dp}}}
		Image{
			image{morda_img_divider_vert}
			layout{dy{fill}}
		}
		Widget{layout{dx{3dp}}}
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
				id{morda_dropdown_mouseproxy}
				layout{
					dx{fill} dy{fill}
				}
			}
			Color{
				id{morda_dropdown_color}
				color{@{morda_color_highlight}}
				visible{false}
				layout{
					dx{fill} dy{fill}
				}
			}
		}
	)qwertyuiop";

const char* contextMenuLayout_c = R"qwertyuiop(
		Pile{
			Widget{
				id{minSizeSpacer}
			}
			NinePatch{
				layout{
					dx{max}
				}
				image{morda_npt_contextmenu_bg}
				Column{
					layout{
						dx{max}
					}
					id{morda_contextmenu_content}
				}
			}
			MouseProxy{
				layout{
					dx{fill} dy{fill}
				}
				id{contextMenuMouseProxy}
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

	auto minSizeSpacer = np->findById("minSizeSpacer");

	auto& lp = minSizeSpacer->getLayoutParams();
	lp.dim.x = this->rect().d.x;

	auto va = np->findByIdAs<morda::Column>("morda_contextmenu_content");
	ASSERT(va)

	for(size_t i = 0; i != this->provider->count(); ++i){
		va->add(this->wrapItem(this->provider->getWidget(i), i));
	}

	this->hoveredIndex = -1;

	np->getByNameAs<MouseProxy>("contextMenuMouseProxy").mouseButton
			= [this](Widget& w, bool isDown, const Vec2r pos, MouseButton_e button, unsigned id) -> bool{
				if(!isDown){
					this->mouseButtonUpHandler(false);
				}

				return true;
			};

	overlay->showContextMenu(np, this->calcPosInParent(Vec2r(0), overlay) + Vec2r(0, this->rect().d.y));
}

bool DropDownSelector::onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID){
	if(!isDown){
		this->mouseButtonUpHandler(true);
	}

	return this->NinePatchPushButton::onMouseButton(isDown, pos, button, pointerID);
}

void DropDownSelector::mouseButtonUpHandler(bool isFirstOne) {
	auto oc = this->findAncestor<Overlay>();
	if(!oc){
		throw Exc("No Overlay found in ancestors of DropDownSelector");
	}

	auto dds = this->sharedFromThis(this);

//	TRACE(<< "DropDownSelector::mouseButtonUpHandler(): this->hoveredIndex = " << this->hoveredIndex << std::endl)
//	TRACE(<< "DropDownSelector::mouseButtonUpHandler(): isFirstOne = " << isFirstOne << std::endl)

	if(this->hoveredIndex < 0){
		if(!isFirstOne){
			morda::Morda::inst().postToUiThread([oc](){
				oc->hideContextMenu();
			});
		}
		return;
	}
	this->setSelection(this->hoveredIndex);

//	TRACE(<< "DropDownSelector::mouseButtonUpHandler(): selection set" << std::endl)

	morda::Morda::inst().postToUiThread([dds, oc](){
//		TRACE(<< "DropDownSelector::mouseButtonUpHandler(): hiding context menu" << std::endl)
		oc->hideContextMenu();
		if(dds->selectionChanged){
//			TRACE(<< "DropDownSelector::mouseButtonUpHandler(): calling selection handler" << std::endl)
			dds->selectionChanged(*dds);
		}
	});
}



DropDownSelector::DropDownSelector(const stob::Node* chain) :
		Widget(chain),
		Button(selectorLayout_c.get()),
		NinePatchPushButton(selectorLayout_c.get()),
		selectionContainer(*this->findByIdAs<Pile>("morda_dropdown_selection"))
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

	auto mp = wd->findByIdAs<MouseProxy>("morda_dropdown_mouseproxy");
	ASSERT(mp)

	auto cl = wd->findByIdAs<Color>("morda_dropdown_color");
	ASSERT(cl)
	auto clWeak = utki::makeWeak(cl);

	wd->add(w);

	mp->hoverChanged = [this, clWeak, index](Widget& w, unsigned id){
		if(auto c = clWeak.lock()){
			c->setVisible(w.isHovered());
		}
		if(w.isHovered()){
			this->hoveredIndex = int(index);
		}else{
			if(this->hoveredIndex > 0 && decltype(index)(this->hoveredIndex) == index){
				this->hoveredIndex = -1;
			}
		}
	};

	return wd;
}
