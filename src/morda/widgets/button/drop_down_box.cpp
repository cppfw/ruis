#include "drop_down_box.hpp"

#include "../../context.hpp"

#include "../group/Overlay.hpp"

#include "../proxy/MouseProxy.hpp"

#include "../button/button.hpp"

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

class static_provider : public drop_down_box::provider{
	std::vector<puu::tree> widgets;
public:

	size_t count() const noexcept override{
		return this->widgets.size();
	}

	std::shared_ptr<widget> get_widget(size_t index)override{
		auto i = std::next(this->widgets.begin(), index);
		ASSERT(this->get_drop_down_box())
		return this->get_drop_down_box()->context->inflater.inflate(i, i + 1);
	}

	void recycle(size_t index, std::shared_ptr<widget> w)override{
//		TRACE(<< "static_provider::recycle(): index = " << index << std::endl)
	}

	void add(puu::tree&& w){
		this->widgets.emplace_back(std::move(w));
	}
};

}

void drop_down_box::showDropdownMenu(){
	if(!this->item_provider){
		return;
	}

	auto overlay = this->find_ancestor<Overlay>();
	if(!overlay){
		throw utki::invalid_state("drop_down_box: no overlay parent found");
	}

	auto np = this->context->inflater.inflate(contextMenuLayout_c);
	ASSERT(np)

	auto minSizeSpacer = np->try_get_widget("minSizeSpacer");

	auto& lp = minSizeSpacer->get_layout_params();
	lp.dims.x = this->rect().d.x;

	auto va = np->try_get_widget_as<morda::Column>("morda_contextmenu_content");
	ASSERT(va)

	for(size_t i = 0; i != this->item_provider->count(); ++i){
		va->push_back(this->wrapItem(this->item_provider->get_widget(i), i));
	}

	this->hoveredIndex = -1;

	np->get_widget_as<MouseProxy>("contextMenuMouseProxy").mouseButton
			= [this](widget& w, bool isDown, const Vec2r pos, mouse_button button, unsigned id) -> bool{
				if(!isDown){
					this->mouseButtonUpHandler(false);
				}

				return true;
			};

	overlay->showContextMenu(np, this->pos_in_ancestor(Vec2r(0), overlay) + Vec2r(0, this->rect().d.y));
}

bool drop_down_box::on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerID){
	if(!isDown){
		this->mouseButtonUpHandler(true);
	}

	return this->NinePatchPushButton::on_mouse_button(isDown, pos, button, pointerID);
}

void drop_down_box::mouseButtonUpHandler(bool isFirstOne){
	auto oc = this->find_ancestor<Overlay>();
	if(!oc){
		throw utki::invalid_state("No overlay found in ancestors of drop_down_box");
	}

	auto dds = this->sharedFromThis(this);

//	TRACE(<< "drop_down_box::mouseButtonUpHandler(): this->hoveredIndex = " << this->hoveredIndex << std::endl)
//	TRACE(<< "drop_down_box::mouseButtonUpHandler(): isFirstOne = " << isFirstOne << std::endl)

	if(this->hoveredIndex < 0){
		if(!isFirstOne){
			this->context->run_from_ui_thread([oc](){
				oc->hideContextMenu();
			});
		}
		return;
	}
	this->set_selection(this->hoveredIndex);

//	TRACE(<< "drop_down_box::mouseButtonUpHandler(): selection set" << std::endl)

	this->context->run_from_ui_thread([dds, oc](){
//		TRACE(<< "drop_down_box::mouseButtonUpHandler(): hiding context menu" << std::endl)
		oc->hideContextMenu();
		if(dds->selection_changed){
//			TRACE(<< "drop_down_box::mouseButtonUpHandler(): calling selection handler" << std::endl)
			dds->selection_changed(*dds);
		}
	});
}

drop_down_box::drop_down_box(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, selectorLayout_c),
		NinePatchPushButton(this->context, selectorLayout_c),
		selectionContainer(*this->try_get_widget_as<Pile>("morda_dropdown_selection"))
{
	this->pressed_changed = [this](button& b){
		if(!b.is_pressed()){
			return;
		}

		this->showDropdownMenu();
	};

	std::shared_ptr<static_provider> pr = std::make_shared<static_provider>();

	for(const auto& p : desc){
		if(is_property(p)){
			continue;
		}

		pr->add(puu::tree(p));
	}

	this->set_provider(std::move(pr));
}

void drop_down_box::set_provider(std::shared_ptr<provider> item_provider){
	if(item_provider && item_provider->dd){
		throw utki::invalid_state("drop_down_box::setItemsProvider(): given provider is already set to some drop_down_box");
	}

	if(this->item_provider){
		this->item_provider->dd = nullptr;
	}
	this->item_provider = std::move(item_provider);
	if(this->item_provider){
		this->item_provider->dd = this;
	}
	this->handleDataSetChanged();
}

void drop_down_box::provider::notify_data_set_changed(){
	if(this->dd){
		this->dd->handleDataSetChanged();
	}
}

void drop_down_box::handleDataSetChanged(){
	this->selectionContainer.clear();

	if(!this->item_provider){
		return;
	}
	if(this->selectedItem_v >= this->item_provider->count()){
		return;
	}

	this->selectionContainer.push_back(this->item_provider->get_widget(this->selectedItem_v));
}

void drop_down_box::set_selection(size_t i){
	this->selectedItem_v = i;

	this->handleDataSetChanged();
}

std::shared_ptr<widget> drop_down_box::wrapItem(std::shared_ptr<widget>&& w, size_t index){
	auto wd = std::dynamic_pointer_cast<Pile>(this->context->inflater.inflate(itemLayout_c));
	ASSERT(wd)

	auto mp = wd->try_get_widget_as<MouseProxy>("morda_dropdown_mouseproxy");
	ASSERT(mp)

	auto cl = wd->try_get_widget_as<Color>("morda_dropdown_color");
	ASSERT(cl)
	auto clWeak = utki::makeWeak(cl);

	wd->push_back(w);

	mp->hoverChanged = [this, clWeak, index](widget& w, unsigned id){
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
