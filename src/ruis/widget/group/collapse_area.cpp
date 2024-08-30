/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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
#include "../button/base/toggle_button.hpp"

using namespace ruis;

namespace {
const auto layout_c = tml::read(R"qwertyuiop(
	layout{column}
	
	@pile{
		lp{dx{max}}
		@color{
			lp{dx{fill}dy{fill}}
			color{${ruis_color_mg}}
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
					id{ruis_switch}
					look{
						unpressed{ruis_img_dropdown_arrow}
						pressed{ruis_img_dropright_arrow}
					}
				}
				@widget{
					lp{dx{${marHor}}}
				}
				@pile{
					id{ruis_title}
				}
			}
		}
	}
	@pile{
		id{ruis_content}
	}
)qwertyuiop");
} // namespace

collapse_area::collapse_area(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc),
	container(this->context, layout_c),
	content_area(this->get_widget_as<container>("ruis_content")),
	title_v(this->get_widget_as<container>("ruis_title"))
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "title") {
			this->title_v.push_back_inflate(p.children);
		}
	}

	this->content_area.push_back_inflate(desc);

	{
		auto& sw = this->get_widget_as<toggle_button>("ruis_switch");
		sw.pressed_change_handler = [this](button& tb) {
			auto& lp = this->content_area.get_layout_params();
			if (tb.is_pressed()) {
				using namespace length_literals;
				lp.dims.y() = 0_px;
			} else {
				lp.dims.y() = dim::min;
			}
		};
	}
}
