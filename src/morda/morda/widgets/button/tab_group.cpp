/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "tab.hpp"
#include "tab_group.hpp"
#include "../../util/util.hpp"

using namespace morda;

tab_group::tab_group(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		choice_group(this->context, desc)
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
					this->set_filler(this->context->loader.load<res::image>(get_property_value(pp).to_string()));
				}
			}
		}
	}

	if(!this->filler){
		this->set_filler(this->context->loader.load<res::image>("morda_img_tabs_filler"));
	}
}

void tab_group::set_filler(std::shared_ptr<res::image> filler){
	this->filler = std::move(filler);
	this->fillerTexture = this->filler->get();
}

morda::vector2 tab_group::measure(const morda::vector2& quotum)const{
	vector2 ret(quotum);
	using std::max;
	ret = max(ret, real(0)); // clamp bottom

	real length = 0;

	sides<real> prevBorders = real(0);

	for(auto& c : this->children()){
		ASSERT(c)
		auto& lp = this->get_layout_params_as_const<container::layout_params>(*c);

		auto t = dynamic_cast<const morda::tab*>(c.get());
		if(!t){
			throw std::logic_error("Non-tab widget added to tab_group, only tab widgets are allowed to be added to tab_group");
		}

		morda::vector2 d;

		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.dims[j] == layout_params::max || lp.dims[j] == layout_params::fill){
				throw std::logic_error("'max' or 'fill' encountered in layout parameters for tab_group container");
			}else if(lp.dims[j] == layout_params::min){
				d[j] = -1;
			}else{
				d[j] = lp.dims[j];
			}
		}

		d = c->measure(d);

		length += d.x();

		using std::min;
		using std::max;

		auto borders = t->get_actual_borders();
		length -= min(prevBorders.right(), borders.left());
		prevBorders = borders;

		if(quotum.y() < 0){
			ret.y() = max(ret.y(), d.y()); // clamp bottom
		}
	}

	if(quotum.x() < 0){
		ret.x() = length;
	}

	return ret;
}

void tab_group::lay_out(){
	real pos = 0;

	sides<real> prevBorders = 0;

	for(auto& c : this->children()){
		ASSERT(c)
		auto& lp = this->get_layout_params_as_const<container::layout_params>(*c);

		auto dim = this->dims_for_widget(*c, lp);
		c->resize(dim);

		auto t = dynamic_cast<morda::tab*>(c.get());
		if(!t){
			throw std::logic_error("Non-tab widget added to tab_group, only tab widgets are allowed to be added to tab_group");
		}

		auto borders = t->get_actual_borders();

		using std::min;
		using std::round;

		pos -= min(prevBorders.right(), borders.left());
		c->move_to(vector2(pos, round((this->rect().d.y() - c->rect().d.y()) / 2)));
		pos += dim.x();

		prevBorders = borders;
	}
}

void tab_group::render(const morda::matrix4& matrix)const{
	for(auto& w: this->children()){
		if(!this->is_active(*w)){
			this->render_child(matrix, *w);
		}
	}

	auto& ab = this->get_active();
	if(ab){
		this->render_child(matrix, *ab);
	}

	// render filler
	if(this->children().size() != 0){
		real ce = this->children().back()->rect().x2();
		real l = this->rect().d.x() - ce;
		if(l > 0){
			matrix4 m(matrix);
			m.translate(ce, this->rect().d.y() - this->fillerTexture->dims.y());
			m.scale(l, this->fillerTexture->dims.y());
			this->fillerTexture->render(m);
		}
	}
}
