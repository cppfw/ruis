/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "collapse_area.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"
#include "../button/toggle_button.hpp"

using namespace morda;

namespace {
const auto layout_c = treeml::read(R"qwertyuiop(
	layout{column}
	
	@pile{
		lp{dx{max}}
		@color{
			lp{dx{fill}dy{fill}}
			color{${morda_color_mg}}
		}
		@margins{
			lp{dx{max}dy{max}}

			defs{
				marHor{4dp}
				marVer{1dp}
			}
			left{${marHor}}
			top{${marVer}}
			right{${marHor}}
			bottom{${marVer}}

			@row{
				lp{dx{max}}
				@image_toggle{
					id{switch}
					look{
						unpressed{morda_img_dropdown_arrow}
						pressed{morda_img_dropright_arrow}
					}
				}
				@widget{
					lp{dx{${marHor}}}
				}
				@pile{
					id{title}
				}
			}
		}
	}
	@pile{
		id{content}
	}
)qwertyuiop");
} // namespace

collapse_area::collapse_area(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	container(this->context, layout_c)
{
	// TODO: use get_widget_as()
	this->contentArea = this->try_get_widget_as<container>("content");
	ASSERT(this->contentArea)

	// TODO: use get_widget_as()
	this->title_v = this->try_get_widget_as<container>("title");
	ASSERT(this->title_v)

	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "title") {
			this->title_v->push_back_inflate(p.children);
		}
	}

	this->contentArea->push_back_inflate(desc);

	{
		auto sw = this->try_get_widget_as<toggle_button>("switch");
		ASSERT(sw)
		sw->press_handler = [this](button& tb) {
			auto& lp = this->contentArea->get_layout_params();
			if (tb.is_pressed()) {
				lp.dims.y() = 0;
			} else {
				lp.dims.y() = layout_params::min;
			}
		};
	}
}
