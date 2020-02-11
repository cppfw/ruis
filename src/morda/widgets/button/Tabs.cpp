#include "../../exception.hpp"

#include "Tab.hpp"
#include "Tabs.hpp"
#include "../../util/util.hpp"


using namespace morda;

Tabs::Tabs(const puu::forest& desc) :
		widget(desc),
		ChoiceGroup(desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "look"){
			for(const auto& pp : p.children){
				if(!is_property(pp)){
					continue;
				}

				if(p.value == "filler"){
					this->setFiller(morda::inst().context->loader.load<ResImage>(get_property_value(pp).to_string()));
				}
			}
		}
	}

	if(!this->filler){
		this->setFiller(morda::inst().context->loader.load<ResImage>("morda_img_tabs_filler"));
	}
}

void Tabs::setFiller(std::shared_ptr<ResImage> filler) {
	this->filler = std::move(filler);
	this->fillerTexture = this->filler->get();
}


morda::Vec2r Tabs::measure(const morda::Vec2r& quotum) const {
	Vec2r ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		utki::clampBottom(ret[i], real(0));
	}

	real length = 0;

	Sidesr prevBorders = real(0);

	for(auto& c : this->children()){
		ASSERT(c)
		auto& lp = this->getLayoutParamsAs<Container::LayoutParams>(*c);

		auto tab = dynamic_cast<const Tab*>(c.get());
		if(!tab){
			throw morda::Exc("Non-Tab widget added to Tabs, only Tab widgets are allowed to be added to Tabs");
		}

		morda::Vec2r d;

		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.dim[j] == LayoutParams::max_c || lp.dim[j] == LayoutParams::fill_c){
				throw LayoutExc("'max' or 'fill' encountered in layout parameters for Tabs container");
			}else if(lp.dim[j] == LayoutParams::min_c){
				d[j] = -1;
			}else{
				d[j] = lp.dim[j];
			}
		}

		d = c->measure(d);

		length += d.x;

		auto borders = tab->getActualBorders();
		length -= std::min(prevBorders.right(), borders.left());
		prevBorders = borders;

		if(quotum.y < 0){
			utki::clampBottom(ret.y, d.y);
		}
	}

	if(quotum.x < 0){
		ret.x = length;
	}

	return ret;
}

void Tabs::lay_out() {
	real pos = 0;

	Sidesr prevBorders = 0;

	for(auto& c : this->children()){
		ASSERT(c)
		auto& lp = this->getLayoutParamsAs<Container::LayoutParams>(*c);

		auto dim = this->dimForWidget(*c, lp);
		c->resize(dim);

		auto tab = dynamic_cast<Tab*>(c.get());
		if(!tab){
			throw morda::Exc("Non-Tab widget added to Tabs, only Tab widgets are allowed to be added to Tabs");
		}

		auto borders = tab->getActualBorders();

		pos -= std::min(prevBorders.right(), borders.left());
		c->move_to(Vec2r(pos, std::round((this->rect().d.y - c->rect().d.y) / 2)));
		pos += dim.x;

		prevBorders = borders;
	}
}

void Tabs::render(const morda::Matr4r& matrix) const {
	for(auto& w: this->children()){
		if(!this->isWidgetActive(*w)){
			this->render_child(matrix, *w);
		}
	}

	auto& ab = this->getActiveButton();
	if(ab){
		this->render_child(matrix, *ab);
	}

	//render filler
	if(this->children().size() != 0){
		real ce = this->children().back()->rect().right();
		real l = this->rect().d.x - ce;
		if(l > 0){
			Matr4r m(matrix);
			m.translate(ce, this->rect().d.y - this->fillerTexture->dims().y);
			m.scale(l, this->fillerTexture->dims().y);
			this->fillerTexture->render(m);
		}
	}
}

