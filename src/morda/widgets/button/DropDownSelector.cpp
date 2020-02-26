#include "DropDownSelector.hpp"

#include "../../context.hpp"

#include "../group/Overlay.hpp"

#include "../proxy/MouseProxy.hpp"

#include "../button/Button.hpp"

#include "../label/Text.hpp"
#include "../label/Color.hpp"
#include "../label/NinePatch.hpp"

using namespace morda;

namespace{

const auto selectorLayout_c = puu::read(R"qwertyuiop(
	layout{dx{max} dy{max}}

	@row{
		layout{dx{max}}
		@pile{
			id{morda_dropdown_selection}
			layout{dx{min}dy{max} weight{1}}
		}
		@widget{layout{dx{3dp}}}
		@image{
			image{morda_img_divider_vert}
			layout{dy{fill}}
		}
		@widget{layout{dx{3dp}}}
		@image{
			image{morda_img_dropdown_arrow}
		}
	}
)qwertyuiop");

const auto itemLayout_c = puu::read(R"qwertyuiop(
		@pile{
			layout{
				dx{max}
			}
			@mouse_proxy{
				id{morda_dropdown_mouseproxy}
				layout{
					dx{fill} dy{fill}
				}
			}
			@color{
				id{morda_dropdown_color}
				color{${morda_color_highlight}}
				visible{false}
				layout{
					dx{fill} dy{fill}
				}
			}
		}
	)qwertyuiop");

const auto contextMenuLayout_c = puu::read(R"qwertyuiop(
		@pile{
			@widget{
				id{minSizeSpacer}
			}
			@nine_patch{
				layout{
					dx{max}
				}
				image{morda_npt_contextmenu_bg}
				@column{
					layout{
						dx{max}
					}
					id{morda_contextmenu_content}
				}
			}
			@mouse_proxy{
				layout{
					dx{fill} dy{fill}
				}
				id{contextMenuMouseProxy}
			}
		}
	)qwertyuiop");


class StaticProvider : public DropDownSelector::ItemsProvider{
	std::vector<puu::tree> widgets;
public:

	size_t count() const noexcept override{
		return this->widgets.size();
	}

	std::shared_ptr<Widget> getWidget(size_t index)override{
		auto i = std::next(this->widgets.begin(), index);
		ASSERT(this->dropDownSelector())
		return this->dropDownSelector()->context->inflater.inflate(i, i + 1);
	}


	void recycle(size_t index, std::shared_ptr<Widget> w)override{
//		TRACE(<< "StaticProvider::recycle(): index = " << index << std::endl)
	}


	void add(puu::tree&& w){
		this->widgets.emplace_back(std::move(w));
	}
};

}

void DropDownSelector::showDropdownMenu() {
	if(!this->provider){
		return;
	}

	auto overlay = this->find_ancestor<Overlay>();
	if(!overlay){
		throw utki::invalid_state("DropDownSelector: no overlay parent found");
	}

	auto np = this->context->inflater.inflate(contextMenuLayout_c);
	ASSERT(np)

	auto minSizeSpacer = np->try_get_widget("minSizeSpacer");

	auto& lp = minSizeSpacer->get_layout_params();
	lp.dims.x = this->rect().d.x;

	auto va = np->try_get_widget_as<morda::Column>("morda_contextmenu_content");
	ASSERT(va)

	for(size_t i = 0; i != this->provider->count(); ++i){
		va->push_back(this->wrapItem(this->provider->getWidget(i), i));
	}

	this->hoveredIndex = -1;

	np->get_widget_as<MouseProxy>("contextMenuMouseProxy").mouseButton
			= [this](Widget& w, bool isDown, const Vec2r pos, mouse_button button, unsigned id) -> bool{
				if(!isDown){
					this->mouseButtonUpHandler(false);
				}

				return true;
			};

	overlay->showContextMenu(np, this->pos_in_ancestor(Vec2r(0), overlay) + Vec2r(0, this->rect().d.y));
}

bool DropDownSelector::on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerID){
	if(!isDown){
		this->mouseButtonUpHandler(true);
	}

	return this->NinePatchPushButton::on_mouse_button(isDown, pos, button, pointerID);
}

void DropDownSelector::mouseButtonUpHandler(bool isFirstOne) {
	auto oc = this->find_ancestor<Overlay>();
	if(!oc){
		throw utki::invalid_state("No overlay found in ancestors of DropDownSelector");
	}

	auto dds = this->sharedFromThis(this);

//	TRACE(<< "DropDownSelector::mouseButtonUpHandler(): this->hoveredIndex = " << this->hoveredIndex << std::endl)
//	TRACE(<< "DropDownSelector::mouseButtonUpHandler(): isFirstOne = " << isFirstOne << std::endl)

	if(this->hoveredIndex < 0){
		if(!isFirstOne){
			this->context->run_from_ui_thread([oc](){
				oc->hideContextMenu();
			});
		}
		return;
	}
	this->setSelection(this->hoveredIndex);

//	TRACE(<< "DropDownSelector::mouseButtonUpHandler(): selection set" << std::endl)

	this->context->run_from_ui_thread([dds, oc](){
//		TRACE(<< "DropDownSelector::mouseButtonUpHandler(): hiding context menu" << std::endl)
		oc->hideContextMenu();
		if(dds->selectionChanged){
//			TRACE(<< "DropDownSelector::mouseButtonUpHandler(): calling selection handler" << std::endl)
			dds->selectionChanged(*dds);
		}
	});
}



DropDownSelector::DropDownSelector(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		Button(this->context, selectorLayout_c),
		NinePatchPushButton(this->context, selectorLayout_c),
		selectionContainer(*this->try_get_widget_as<Pile>("morda_dropdown_selection"))
{
	this->pressedChanged = [this](Button& b){
		if(!b.isPressed()){
			return;
		}

		this->showDropdownMenu();
	};

	std::shared_ptr<StaticProvider> pr = std::make_shared<StaticProvider>();

	for(const auto& p : desc){
		if(is_property(p)){
			continue;
		}

		pr->add(puu::tree(p));
	}

	this->setItemsProvider(std::move(pr));
}

void DropDownSelector::setItemsProvider(std::shared_ptr<ItemsProvider> provider){
	if(provider && provider->dd){
		throw utki::invalid_state("DropDownSelector::setItemsProvider(): given provider is already set to some DropDownSelector");
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
	this->selectionContainer.clear();

	if(!this->provider){
		return;
	}
	if(this->selectedItem_v >= this->provider->count()){
		return;
	}

	this->selectionContainer.push_back(this->provider->getWidget(this->selectedItem_v));
}

void DropDownSelector::setSelection(size_t i){
	this->selectedItem_v = i;

	this->handleDataSetChanged();
}

std::shared_ptr<Widget> DropDownSelector::wrapItem(std::shared_ptr<Widget>&& w, size_t index) {
	auto wd = std::dynamic_pointer_cast<Pile>(this->context->inflater.inflate(itemLayout_c));
	ASSERT(wd)

	auto mp = wd->try_get_widget_as<MouseProxy>("morda_dropdown_mouseproxy");
	ASSERT(mp)

	auto cl = wd->try_get_widget_as<Color>("morda_dropdown_color");
	ASSERT(cl)
	auto clWeak = utki::makeWeak(cl);

	wd->push_back(w);

	mp->hoverChanged = [this, clWeak, index](Widget& w, unsigned id){
		if(auto c = clWeak.lock()){
			c->set_visible(w.is_hovered());
		}
		if(w.is_hovered()){
			this->hoveredIndex = int(index);
		}else{
			if(this->hoveredIndex > 0 && decltype(index)(this->hoveredIndex) == index){
				this->hoveredIndex = -1;
			}
		}
	};

	return wd;
}
