#include "../../context.hpp"

#include "../../util/util.hpp"
#include "CollapseArea.hpp"
#include "../button/ToggleButton.hpp"

using namespace morda;

namespace{
const auto layout_c = puu::read(R"qwertyuiop(
	Pile{
		layout{dx{max}}
		Color{
			layout{dx{fill}dy{fill}}
			color{${morda_color_mg}}
		}
		Margins{
			layout{dx{max}dy{max}}

			defs{
				marHor{4dp}
				marVer{1dp}
			}
			left{${marHor}}
			top{${marVer}}
			right{${marHor}}
			bottom{${marVer}}

			Row{
				layout{dx{max}}
				ImageToggle{
					id{switch}
					look{
						unpressed{morda_img_dropdown_arrow}
						pressed{morda_img_dropright_arrow}
					}
				}
				Widget{
					layout{dx{${marHor}}}
				}
				Pile{
					id{title}
				}
			}
		}
	}
	Pile{
		id{content}
	}
)qwertyuiop");
}

CollapseArea::CollapseArea(const puu::forest& desc) :
		widget(desc),
		Column(layout_c)
{
	this->contentArea = this->try_get_widget_as<Pile>("content");
	ASSERT(this->contentArea)

	this->title_v = this->try_get_widget_as<Pile>("title");
	ASSERT(this->title_v)

	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "title"){
			this->title_v->inflate_push_back(p.children);
		}
	}

	this->contentArea->inflate_push_back(desc);

	{
		auto sw = this->try_get_widget_as<ToggleButton>("switch");
		ASSERT(sw)
		sw->pressedChanged = [this](Button& tb){
			if(tb.isPressed()){
				this->contentArea->get_layout_params().dim.y = 0;
			}else{
				this->contentArea->get_layout_params().dim.y = Widget::LayoutParams::min_c;
			}
		};
	}
}
